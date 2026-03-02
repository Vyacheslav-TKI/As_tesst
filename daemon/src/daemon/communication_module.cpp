#include "communication_module.h"
#include "../database/database.h"
#include "utils.h"
#include "file_watcher.h"
#include "auth_manager.h"
#include "json_protocol.h"
#include "../common/json.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
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

int CommunicationModule::get_client_fd() const {
    if (!client_connected_ || !ssl_) return -1;
    return SSL_get_fd(ssl_); // возвращает underlying file descriptor
}

void CommunicationModule::background_integrity_worker(std::vector<MonitoredFile> files) {
    for (const auto& file : files) {
        std::string current_hash = hasher_->compute_hash(file.path, file.algorithm);
        json event;
        if (current_hash != file.baseline_hash) {
            // Файл изменён — отправляем событие
            event = JsonProtocol::make_file_changed_event(
                file.file_id, file.path, current_hash
            );
        } else {
            event = JsonProtocol::make_file_unchanged_event(
                file.file_id, file.path, current_hash
            );
        }
        // Отправка через TLS — НЕБЛОКИРУЮЩАЯ
        send_json(event);
    }
    background_check_active_ = false;
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

    if (SSL_CTX_use_certificate_file(ctx_, "/etc/celcon/cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx_, "/etc/celcon/key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        syslog(LOG_ERR, "socket failed");
        exit(EXIT_FAILURE);
    }

    // Сделать серверный сокет non-blocking
    int flags = fcntl(server_fd_, F_GETFL, 0);
    fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK);

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
    if (listen(server_fd_, 1) < 0) {
        syslog(LOG_ERR, "listen failed");
        exit(EXIT_FAILURE);
    }
}

void CommunicationModule::start() {
    // Слушаем, но не принимаем сразу — accept() будет в handle_incoming()
}

// communication_module.cpp — обновлённые обработчики

json CommunicationModule::process_auth(const json& req) {
    std::string login = req.value("login", "");
    std::string password = req.value("token", ""); // в ТЗ — token, но на самом деле пароль при входе
    auto sid = auth_manager_.authenticate(login, password);
    if (sid) {
        User user = user_dao_->get_by_login(login).value();
        return {{"code", 200}, {"answ", "ok"}, {"session_id", *sid}, {"fio", user.fio}, {"post", user.post}, {"role", user.role}};
    } else {
        return {{"code", 401}, {"answ", "unauthorized"}};
    }
}

json CommunicationModule::process_logout(const json& req) {
    auth_manager_.logout(req.value("session_id", ""));
    return {{"code", 200}, {"answ", "ok"}};
}

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
        if (file.for_users.empty()) {
            file.for_users = "0";
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

    // Запускаем фоновую проверку, если ещё не запущена
    if (!background_check_active_.exchange(true)) {
        // Передаём копию файлов в поток
        background_check_thread_ = std::thread(
            &CommunicationModule::background_integrity_worker, this, files
        );
        background_check_thread_.detach(); // или join при отключении
    }

    return JsonProtocol::make_sync_response(files);
}

json CommunicationModule::process_list_users(const json& req) {
    auto parsed = JsonProtocol::parse_list_users(req);
    if (!parsed) {
        return JsonProtocol::make_error(400, "invalid LIST_USERS request");
    }

    if (!auth_manager_.validate_session(parsed->session_id, /*min_role=*/1)) {
        return JsonProtocol::make_error(403, "only users since level 1 can list users");
    }

   std::vector<User> users = user_dao_->get_all_users();

    json users_array = json::array();
    for (const User& user : users) {
        users_array.push_back({
            {"id", user.id},
            {"fio", user.fio},
            {"post", user.post},
            {"role", user.role}
        });
    }

    return json{
        {"code", 200},
        {"answ", "ok"},
        {"users", users_array}
    };;
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
    if (parsed->token.size() < 8) {
        return JsonProtocol::make_error(400, "Пароль должен содержать минимум 8 символов.");
    }
    if (parsed->level < 0 || parsed->level > 2) {
        return JsonProtocol::make_error(400, "Invalid user level.");
    }

    if (user_dao_->login_exists(parsed->login)) {
        return JsonProtocol::make_error(409, "Такой пользователь уже существует. Пожалуйста, выберите другие данные.");
    }

    // Хешируем пароль (пример — SHA256, но лучше bcrypt)
    std::string hashed = hash_password(parsed->token); // реализуй отдельно

    User new_user{
        .login = parsed->login,
        .password_hash = hashed,
        .role = parsed->level,
        .fio = parsed->fio,         // можно запросить, но в ТЗ не обязательны при ADD_USER
        .post = parsed->post
    };

    if (!user_dao_->add_user(new_user)) {
        return JsonProtocol::make_error(500, "failed to register user");
    }

    return JsonProtocol::make_success();
}

json CommunicationModule::process_delete_user(const json& req) {
    auto parsed = JsonProtocol::parse_delete_user(req);
    if (!parsed) {
        return JsonProtocol::make_error(400, "invalid DELETE_USER request");
    }

    // Только администратор (роль = 2)
    if (!auth_manager_.validate_session(parsed->session_id, /*min_role=*/2)) {
        return JsonProtocol::make_error(403, "only admin can delete users");
    }

    if (!user_dao_->delete_user(parsed->user_id)) {
        return JsonProtocol::make_error(500, "failed to delete user");
    }

    return JsonProtocol::make_success();
}

void CommunicationModule::handle_incoming() {
    if (!client_connected_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return; // Нет входящих подключений (non-blocking)
            }
            syslog(LOG_ERR, "accept() failed: %m");
            return;
        }

        ssl_ = SSL_new(ctx_);
        if (!ssl_) {
            close(client_fd);
            return;
        }

        SSL_set_fd(ssl_, client_fd);
        if (SSL_accept(ssl_) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl_);
            ssl_ = nullptr;
            close(client_fd);
            return;
        }

        recv_buffer_.clear();
        client_connected_ = true;
        syslog(LOG_INFO, "Client connected via TLS");
        return;
    }

    // Чтение от существующего клиента
    char buffer[4096];
    int bytes = SSL_read(ssl_, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        int err = SSL_get_error(ssl_, bytes);
        if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SSL || err == SSL_ERROR_SYSCALL) {
            // Клиент закрыл соединение или ошибка
            syslog(LOG_INFO, "Client disconnected");
        }

        SSL_free(ssl_);
        ssl_ = nullptr;
        client_connected_ = false; // ← ОБЯЗАТЕЛЬНО сбросить!
        recv_buffer_.clear();
        return;
    }

    buffer[bytes] = '\0';
    recv_buffer_.append(buffer, bytes);

    size_t pos;
    while ((pos = recv_buffer_.find('\n')) != std::string::npos) {
        std::string line = recv_buffer_.substr(0, pos);
        recv_buffer_.erase(0, pos + 1);

        if (!line.empty()) {
            handle_command(line);
        }
    }
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
           send_json(process_auth(j));
        }
        else if (*cmd == "ADD_FILES") {
            send_json(process_add_files(j));
        }
        else if (*cmd == "ADD_USER") {
            send_json(process_add_user(j));
        }
        else if (*cmd == "DELETE_USER") {
            send_json(process_delete_user(j));
        }
        else if (*cmd == "SYNC") {
            send_json(process_sync(j));
        }
        else if (*cmd == "LIST_USERS") {
            send_json(process_list_users(j));
        }
        else if (*cmd == "LOGOUT") {
            send_json(process_logout(j));
        }
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "JSON error: %s", e.what());
        send_json(JsonProtocol::make_error(400, "invalid json"));
    }
}

// Аналогично реализуются ADD_FILES, ADD_USER и т.д.
// При ADD_FILES — проверяешь роль ≥ 1, вставляешь в БД, обновляешь file_watcher

void CommunicationModule::send_json(const json& j) {
    if (!client_connected_ || !ssl_) return;
    std::string s = j.dump() + "\n";
    int written = SSL_write(ssl_, s.c_str(), s.size());
    if (written <= 0) {
        int err = SSL_get_error(ssl_, written);
        if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
            // Можно повторить позже (в non-blocking режиме)
            syslog(LOG_WARNING, "SSL_write would block");
        } else {
            syslog(LOG_ERR, "SSL_write failed");
            // Отключить клиента
            client_connected_ = false;
            SSL_free(ssl_);
            ssl_ = nullptr;
        }
    }
}
