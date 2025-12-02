#include "json_protocol.h"
#include "../database/entities.h"
#include <stdexcept>

std::optional<std::string> JsonProtocol::parse_command(const json& j) {
    if (!j.contains("cmd") || !j["cmd"].is_string()) return std::nullopt;
    return j["cmd"].get<std::string>();
}

std::optional<AuthRequest> JsonProtocol::parse_auth(const json& j) {
    if (!j.contains("login") || !j.contains("token")) return std::nullopt;
    if (!j["login"].is_string() || !j["token"].is_string()) return std::nullopt;
    return AuthRequest{
        .login = j["login"],
        .token = j["token"]
    };
}

std::optional<AddFilesRequest> JsonProtocol::parse_add_files(const json& j) {
    if (!j.contains("session_id") || !j["session_id"].is_string()) return std::nullopt;
    if (!j.contains("files") || !j["files"].is_array()) return std::nullopt;

    std::vector<MonitoredFile> files;
    for (const auto& f : j["files"]) {
        if (!f.contains("path") || !f.contains("alg") || !f.contains("hash") || !f.contains("for_users"))
            continue;
        MonitoredFile mf;
        mf.file_id = f.value("id", -1); // при добавлении может не быть ID
        mf.path = f["path"];
        mf.algorithm = f["alg"];
        mf.baseline_hash = f["hash"];
        // "for_users" — строка вроде "0" или "1,2,3"
        // пока просто сохраняем как строку, БД сама хранит TEXT
        // но для внутреннего use можно разобрать — но пока не нужно
        files.push_back(mf);
    }
    return AddFilesRequest{
        .session_id = j["session_id"],
        .files = std::move(files)
    };
}

std::optional<SyncRequest> JsonProtocol::parse_sync(const json& j) {
    if (!j.contains("session_id") || !j["session_id"].is_string()) return std::nullopt;
    return SyncRequest{.session_id = j["session_id"]};
}

std::optional<AddUserRequest> JsonProtocol::parse_add_user(const json& j) {
    if (!j.contains("session_id") || !j.contains("login") || !j.contains("token") || !j.contains("level"))
        return std::nullopt;
    if (!j["login"].is_string() || !j["token"].is_string() || !j["level"].is_number_integer())
        return std::nullopt;

    // Опциональные поля: если нет — пустая строка
    std::string fio  = j.value("fio", "");
    std::string post = j.value("post", "");

    return AddUserRequest{
        .session_id = j["session_id"],
        .login = j["login"],
        .token = j["token"], // в протоколе "token", но это пароль
        .fio = fio,
        .post = post,
        .level = j["level"]
    };
}

// --- Формирование ответов ---

json JsonProtocol::make_success() {
    return json{{"code", 200}, {"answ", "ok"}};
}

json JsonProtocol::make_error(int code, const std::string& message) {
    return json{{"code", code}, {"answ", message}};
}

json JsonProtocol::make_auth_success(const std::string& session_id) {
    return json{{"code", 200}, {"session_id", session_id}, {"answ", "ok"}};
}

json JsonProtocol::make_sync_response(const std::vector<MonitoredFile>& files) {
    json arr = json::array();
    for (const auto& f : files) {
        arr.push_back({
            {"id", f.file_id},
            {"path", f.path}
        });
    }
    return json{{"code", 200}, {"answ", "ok"}, {"files", arr}};
}

// --- События ---

json JsonProtocol::make_file_changed_event(int file_id, const std::string& path, const std::string& new_hash) {
    return json{
        {"event", "FILE_CHANGED"},
        {"id", file_id},
        {"path", path},
        {"hash", new_hash}
    };
}

json JsonProtocol::make_session_expired_event(const std::string& session_id) {
    return json{
        {"event", "SESSION_EXPIRED"},
        {"session_id", session_id}
    };
}
