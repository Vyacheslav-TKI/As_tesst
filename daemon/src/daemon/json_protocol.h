#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>
#include "../database/entities.h"

using json = nlohmann::json;

struct AuthRequest {
    std::string login;
    std::string token; // на самом деле — пароль при входе
};

struct AddFilesRequest {
    std::string session_id;
    std::vector<MonitoredFile> files;
};

struct ChangeFilesRequest {
    std::string session_id;
    std::vector<MonitoredFile> files;
};

struct DeleteFilesRequest {
    std::string session_id;
    std::vector<int> file_ids; // или vector<string path>, но по ТЗ — id
};

struct SyncRequest {
    std::string session_id;
};

struct StatRequest {
    std::string session_id;
    time_t date_begin;
    time_t date_end;
};

struct AddUserRequest {
    std::string session_id;
    std::string login;
    std::string password; // token в протоколе — но это пароль
    int level; // 0, 1, 2
};

struct LogoutRequest {
    std::string session_id;
};

class JsonProtocol {
public:
    // === Парсинг входящих команд ===
    static std::optional<std::string> parse_command(const json& j);
    static std::optional<AuthRequest> parse_auth(const json& j);
    static std::optional<AddFilesRequest> parse_add_files(const json& j);
    static std::optional<ChangeFilesRequest> parse_change_files(const json& j);
    static std::optional<DeleteFilesRequest> parse_delete_files(const json& j);
    static std::optional<SyncRequest> parse_sync(const json& j);
    static std::optional<StatRequest> parse_stat(const json& j);
    static std::optional<AddUserRequest> parse_add_user(const json& j);
    static std::optional<LogoutRequest> parse_logout(const json& j);

    // === Формирование ответов ===
    static json make_success();
    static json make_error(int code, const std::string& message);
    static json make_auth_success(const std::string& session_id);
    static json make_sync_response(const std::vector<MonitoredFile>& files);
    // другие по мере надобности

    // === Формирование событий (push от демона) ===
    static json make_file_changed_event(int file_id, const std::string& path, const std::string& new_hash);
    static json make_session_expired_event(const std::string& session_id);
};
