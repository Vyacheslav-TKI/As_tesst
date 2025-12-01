// monitored_file_dao.cpp — ключевой метод get_for_user
#include "monitored_file_dao.h"
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
