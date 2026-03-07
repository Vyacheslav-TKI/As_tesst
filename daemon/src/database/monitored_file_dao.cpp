// monitored_file_dao.cpp — ключевой метод get_for_user
#include "monitored_file_dao.h"
#include <syslog.h>
std::vector<MonitoredFile> MonitoredFileDAO::get_for_user(int user_id) {
    // Запрос: все файлы, где ForUsers = '0' ИЛИ ForUsers содержит user_id
    const char* sql =
        "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
        "FROM MonitoredFiles "
        "WHERE ForUsers = '0' OR ForUsers LIKE ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    std::string pattern = "%," + std::to_string(user_id) + ",%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<MonitoredFile> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);
        f.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.algorithm = sqlite3_column_int(stmt, 3);
        f.baseline_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        result.push_back(f);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool MonitoredFileDAO::add_file(const MonitoredFile& file) {
    const char* sql = "INSERT INTO MonitoredFiles (FilePath, ForUsers, HashAlgorithm, Hash) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, file.path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, file.for_users.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, file.algorithm);
    sqlite3_bind_text(stmt, 4, file.baseline_hash.c_str(), -1, SQLITE_STATIC);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    syslog(LOG_DEBUG, sqlite3_errmsg(db_));
    return ok;
}

std::vector<MonitoredFile> MonitoredFileDAO::get_all() {
    const char* sql = "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash FROM MonitoredFiles";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }
    std::vector<MonitoredFile> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);
        f.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.algorithm = sqlite3_column_int(stmt, 3);
        f.baseline_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        result.push_back(f);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<MonitoredFile> MonitoredFileDAO::get_by_id_for_user(int file_id, int user_id) {
    const char* sql =
        "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
        "FROM MonitoredFiles "
        "WHERE FileID = '?' AND ForUsers = '0' OR ForUsers LIKE ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, file_id);

    std::string pattern = "%," + std::to_string(user_id) + ",%";
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return std::nullopt;
    }

    MonitoredFile f;
    f.file_id = sqlite3_column_int(stmt, 0);
    if (sqlite3_column_text(stmt, 1))
        f.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    if (sqlite3_column_text(stmt, 2))
        f.for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    f.algorithm = sqlite3_column_int(stmt, 3);
    if (sqlite3_column_text(stmt, 4))
        f.baseline_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    sqlite3_finalize(stmt);
    return f;
}

std::optional<MonitoredFile> MonitoredFileDAO::get_by_id(int file_id) {
    const char* sql =
        "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
        "FROM MonitoredFiles "
        "WHERE FileID = '?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, file_id);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return std::nullopt;
    }

    MonitoredFile f;
    f.file_id = sqlite3_column_int(stmt, 0);
    if (sqlite3_column_text(stmt, 1))
        f.path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    if (sqlite3_column_text(stmt, 2))
        f.for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    f.algorithm = sqlite3_column_int(stmt, 3);
    if (sqlite3_column_text(stmt, 4))
        f.baseline_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    sqlite3_finalize(stmt);
    return f;
}

std::unordered_map<int, std::string> MonitoredFileDAO::get_files_by_ids_for_user(
    const std::set<int>& file_ids, int user_id) {

    std::unordered_map<int, std::string> result;

    if (file_ids.empty()) {
        return result;
    }

    // Строим SQL с плейсхолдерами для всех ID
    std::string sql =
        "SELECT FileID, FilePath, ForUsers "
        "FROM MonitoredFiles "
        "WHERE FileID IN (";

    // Добавляем плейсхолдеры для каждого ID
    for (size_t i = 0; i < file_ids.size(); ++i) {
        if (i > 0) sql += ",";
        sql += "?";
    }

    sql += ") AND (ForUsers = '0' OR ForUsers LIKE ?)";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    // Биндим все file_id
    int index = 1;
    for (int file_id : file_ids) {
        sqlite3_bind_int(stmt, index++, file_id);
    }

    // Биндим паттерн для ForUsers
    std::string pattern = "%," + std::to_string(user_id) + ",%";
    sqlite3_bind_text(stmt, index, pattern.c_str(), -1, SQLITE_TRANSIENT);

    // Получаем результаты
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int file_id = sqlite3_column_int(stmt, 0);
        if (sqlite3_column_text(stmt, 1)) {
            result[file_id] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        }
    }

    sqlite3_finalize(stmt);
    return result;
}
