#include "communication_module.h"
#include "../database/database.h"
#include "utils.h"
#include "file_watcher.h"
#include "auth_manager.h"
#include "json_protocol.h"
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <syslog.h>
#include <cstring>

using json = nlohmann::json;

CommunicationModule::CommunicationModule(
    Database& db,
    IntegrityChecker* hasher,
    UserDAO* user_dao,
    MonitoredFileDAO* file_dao,
    File_watcher& watcher
)
    : hasher_(hasher)
    , user_dao_(user_dao)
    , file_dao_(file_dao)
    , watcher_(watcher)
    , db_(db)
    , auth_manager_(user_dao)
{
    init_tls();
}

void CommunicationModule::init_tls() {
    SSL_library_init();
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD* method = TLS_server_method();
    ctx_ = SSL_CTX_new(method);
    if (!ctx_) {
        syslog(LOG_ERR, "SSL_CTX_new failed");
        exit(EXIT_FAILURE);
    }

    // Загружаем сертификат и ключ (пути можно задать в конфиге)
    if (SSL_CTX_use_certificate_file(ctx_, "/etc/celcon/cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx_, "/etc/celcon/key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    int enable = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        syslog(LOG_ERR, "bind failed");
        exit(EXIT_FAILURE);
    }
    listen(server_fd_, 1);
}

void CommunicationModule::start() {
    // Слушаем, но не принимаем сразу — accept() будет в handle_incoming()
}

// communication_module.cpp — обновлённые обработчики

json CommunicationModule::process_add_files(const json& req) {
    auto parsed = JsonProtocol::parse_add_files(req);
    if (!parsed) {
        return JsonProtocol::make_error(400, "invalid ADD_FILES request");
    }

    if (!auth_manager_.validate_session(parsed->session_id, /*min_role=*/1)) {
        return JsonProtocol::make_error(403, "forbidden: insufficient role");
    }

    auto user_opt = auth_manager_.get_user_by_session(parsed->session_id);
    if (!user_opt) {
        return JsonProtocol::make_error(401, "session expired");
    }

    for (auto& file : parsed->files) {
        // Вычисляем эталонный хеш, если не передан (или проверяем согласованность)
        if (file.baseline_hash.empty()) {
            file.baseline_hash = hasher_->compute_hash(file.path, file.algorithm);
            if (file.baseline_hash.empty()) {
                return JsonProtocol::make_error(500, "failed to compute hash for " + file.path);
            }
        }

        // Сохраняем в БД
        if (!file_dao_->add_file(file)) {
            return JsonProtocol::make_error(500, "failed to store file in DB");
        }
    }

    // Обновляем FileWatcher (перезагружаем список файлов)
    auto all_files = file_dao_->get_all();
    watcher_.setFiles(all_files);

    return JsonProtocol::make_success();
}

json CommunicationModule::process_sync(const json& req) {
    auto parsed = JsonProtocol::parse_sync(req);
    if (!parsed || !auth_manager_.validate_session(parsed->session_id)) {
        return JsonProtocol::make_error(401, "unauthorized");
    }

    auto user_opt = auth_manager_.get_user_by_session(parsed->session_id);
    if (!user_opt) {
        return JsonProtocol::make_error(401, "session expired");
    }

    // Только файлы, доступные этому пользователю
    auto files = file_dao_->get_for_user(user_opt->id);
    return JsonProtocol::make_sync_response(files);
}

json CommunicationModule::process_add_user(const json& req) {
    auto parsed = JsonProtocol::parse_add_user(req);
    if (!parsed) {
        return JsonProtocol::make_error(400, "invalid ADD_USER request");
    }

    // Только администратор (роль = 2)
    if (!auth_manager_.validate_session(parsed->session_id, /*min_role=*/2)) {
        return JsonProtocol::make_error(403, "only admin can add users");
    }

    // Валидация входных данных по ТЗ
    if (parsed->login.size() < 2 || !std::all_of(parsed->login.begin(), parsed->login.end(), ::isalpha)) {
        return JsonProtocol::make_error(400, "Введите корректный логин.");
    }
    if (parsed->password.size() < 8) {
        return JsonProtocol::make_error(400, "Пароль должен содержать минимум 8 символов.");
    }
    if (parsed->level < 0 || parsed->level > 2) {
        return JsonProtocol::make_error(400, "Invalid user level.");
    }

    if (user_dao_->login_exists(parsed->login)) {
        return JsonProtocol::make_error(409, "Такой пользователь уже существует. Пожалуйста, выберите другие данные.");
    }

    // Хешируем пароль (пример — SHA256, но лучше bcrypt)
    std::string hashed = hash_password(parsed->password); // реализуй отдельно

    User new_user{
        .login = parsed->login,
        .password_hash = hashed,
        .role = parsed->level,
        .fio = "N/A",         // можно запросить, но в ТЗ не обязательны при ADD_USER
        .post = "N/A"
    };

    if (!user_dao_->add_user(new_user)) {
        return JsonProtocol::make_error(500, "failed to register user");
    }

    return JsonProtocol::make_success();
}

void CommunicationModule::handle_incoming() {
    if (!client_connected_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) return;

        ssl_ = SSL_new(ctx_);
        SSL_set_fd(ssl_, client_fd);
        if (SSL_accept(ssl_) <= 0) {
            ERR_print_errors_fp(stderr);
            close(client_fd);
            return;
        }
        client_connected_ = true;
        syslog(LOG_INFO, "Client connected via TLS");
    }

    char buffer[4096];
    int bytes = SSL_read(ssl_, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        // Клиент отключился
        SSL_shutdown(ssl_);
        SSL_free(ssl_);
        ssl_ = nullptr;
        client_connected_ = false;
        return;
    }
    buffer[bytes] = '\0';
    handle_command(std::string(buffer));
}

void CommunicationModule::handle_command(const std::string& raw) {
    try {
        auto j = json::parse(raw);
        auto cmd = JsonProtocol::parse_command(j);
        if (!cmd) {
            send_json(JsonProtocol::make_error(400, "missing or invalid 'cmd'"));
            return;
        }

        json response;

        if (*cmd == "AUTH") {
            auto req = JsonProtocol::parse_auth(j);
            if (!req) {
                response = JsonProtocol::make_error(400, "invalid auth request");
            } else {
                auto sid = auth_manager_.authenticate(req->login, req->token);
                if (sid) {
                    response = JsonProtocol::make_auth_success(*sid);
                } else {
                    response = JsonProtocol::make_error(401, "unauthorized");
                }
            }
        }
        else if (*cmd == "SYNC") {
            auto req = JsonProtocol::parse_sync(j);
            if (!req || !auth_manager_.validate_session(req->session_id)) {
                response = JsonProtocol::make_error(401, "session invalid");
            } else {
                auto files = db_.load_monitored_files();
                response = JsonProtocol::make_sync_response(files);
            }
        }
        // ... другие команды аналогично

        send_json(response);
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "JSON error: %s", e.what());
        send_json(JsonProtocol::make_error(400, "invalid json"));
    }
}

json CommunicationModule::process_auth(const json& req) {
    std::string login = req.value("login", "");
    std::string password = req.value("password", ""); // в ТЗ — token, но на самом деле пароль при входе
    auto sid = auth_manager_.authenticate(login, password);
    if (sid) {
        return {{"code", 200}, {"session_id", *sid}, {"answ", "ok"}};
    } else {
        return {{"code", 401}, {"answ", "unauthorized"}};
    }
}

// Аналогично реализуются ADD_FILES, ADD_USER и т.д.
// При ADD_FILES — проверяешь роль ≥ 1, вставляешь в БД, обновляешь file_watcher

void CommunicationModule::send_json(const json& j) {
    if (!client_connected_ || !ssl_) return;
    std::string s = j.dump() + "\n";
    SSL_write(ssl_, s.c_str(), s.size());
}
