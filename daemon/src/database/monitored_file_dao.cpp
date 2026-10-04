// monitored_file_dao.cpp — ключевой метод get_for_user
#include "monitored_file_dao.h"
#include <syslog.h>

std::vector<MonitoredFile> MonitoredFileDAO::get_for_user(int user_id) {
    if (!db_) {
        syslog(LOG_ERR, "get_for_user: db_ is null");
        return {};
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_for_user: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return {};
    }

    const char* sql =
    "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
    "FROM MonitoredFiles "
    "WHERE ForUsers = '0' OR ForUsers LIKE ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_for_user prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return {};
    }

    std::string pattern = "%," + std::to_string(user_id) + ",%";
    if (sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        syslog(LOG_ERR, "get_for_user bind failed: %s", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return {};
    }

    std::vector<MonitoredFile> result;
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);

        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.path = path ? path : "";

        const char* for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.for_users = for_users ? for_users : "";

        f.algorithm = sqlite3_column_int(stmt, 3);

        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        f.baseline_hash = hash ? hash : "";

        result.push_back(std::move(f));
    }

    if (rc != SQLITE_DONE) {
        syslog(LOG_ERR, "get_for_user step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    return result;
}

bool MonitoredFileDAO::delete_files(const std::vector<int>& file_ids) {
    if (!db_) {
        syslog(LOG_ERR, "delete_files: db_ is null");
        return false;
    }
    if (file_ids.empty()) {
        return true; // нечего удалять
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "delete_files: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return false;
    }

    std::string sql = "DELETE FROM MonitoredFiles WHERE FileID IN (";
    for (size_t i = 0; i < file_ids.size(); ++i) {
        sql += (i + 1 < file_ids.size() ? "?, " : "?");
    }
    sql += ");";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "delete_files prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }

    for (size_t i = 0; i < file_ids.size(); ++i) {
        if (sqlite3_bind_int(stmt, static_cast<int>(i) + 1, file_ids[i]) != SQLITE_OK) {
            syslog(LOG_ERR, "delete_files bind failed: %s", sqlite3_errmsg(db_));
            sqlite3_finalize(stmt);
            sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
            return false;
        }
    }

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        ok = true;
    } else {
        syslog(LOG_ERR, "delete_files step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);

    if (ok) {
        if (sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
            syslog(LOG_ERR, "delete_files: COMMIT failed: %s", sqlite3_errmsg(db_));
            return false;
        }
    } else {
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
    }

    return ok;
}

bool MonitoredFileDAO::add_file(const MonitoredFile& file) {
    if (!db_) {
        syslog(LOG_ERR, "add_file: db_ is null");
        return false;
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "add_file: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return false;
    }

    const char* sql = "INSERT INTO MonitoredFiles (FilePath, ForUsers, HashAlgorithm, Hash) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "add_file prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, file.path.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, file.for_users.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, file.algorithm) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, file.baseline_hash.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        syslog(LOG_ERR, "add_file bind failed: %s", sqlite3_errmsg(db_));
    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
    return false;
        }

        bool ok = false;
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            ok = true;
        } else {
            syslog(LOG_ERR, "add_file step failed: %s", sqlite3_errmsg(db_));
        }

        sqlite3_finalize(stmt);

        if (ok) {
            if (sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
                syslog(LOG_ERR, "add_file: COMMIT failed: %s", sqlite3_errmsg(db_));
                return false;
            }
        } else {
            sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        }

        return ok;
}

std::vector<MonitoredFile> MonitoredFileDAO::get_all() {
    if (!db_) {
        syslog(LOG_ERR, "get_all: db_ is null");
        return {};
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_all: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return {};
    }

    const char* sql = "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash FROM MonitoredFiles";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_all prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return {};
    }

    std::vector<MonitoredFile> result;
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);

        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.path = path ? path : "";

        const char* for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.for_users = for_users ? for_users : "";

        f.algorithm = sqlite3_column_int(stmt, 3);

        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        f.baseline_hash = hash ? hash : "";

        result.push_back(std::move(f));
    }

    if (rc != SQLITE_DONE) {
        syslog(LOG_ERR, "get_all step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    return result;
}

std::optional<MonitoredFile> MonitoredFileDAO::get_by_id_for_user(int file_id, int user_id) {
    if (!db_) {
        syslog(LOG_ERR, "get_by_id_for_user: db_ is null");
        return std::nullopt;
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id_for_user: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return std::nullopt;
    }

    // ИСПРАВЛЕНО: убраны кавычки вокруг '?' и добавлены скобки для корректного приоритета операторов
    const char* sql =
    "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
    "FROM MonitoredFiles "
    "WHERE FileID = ? AND (ForUsers = '0' OR ForUsers LIKE ?)";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id_for_user prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return std::nullopt;
    }

    if (sqlite3_bind_int(stmt, 1, file_id) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id_for_user bind file_id failed: %s", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return std::nullopt;
    }

    std::string pattern = "%," + std::to_string(user_id) + ",%";
    if (sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id_for_user bind pattern failed: %s", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return std::nullopt;
    }

    std::optional<MonitoredFile> result;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);

        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.path = path ? path : "";

        const char* for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.for_users = for_users ? for_users : "";

        f.algorithm = sqlite3_column_int(stmt, 3);

        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        f.baseline_hash = hash ? hash : "";

        result = std::move(f);
    } else if (rc != SQLITE_DONE) {
        syslog(LOG_ERR, "get_by_id_for_user step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    return result;
}

std::optional<MonitoredFile> MonitoredFileDAO::get_by_id(int file_id) {
    if (!db_) {
        syslog(LOG_ERR, "get_by_id: db_ is null");
        return std::nullopt;
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return std::nullopt;
    }

    // ИСПРАВЛЕНО: убраны некорректные кавычки вокруг параметра '?'
    const char* sql =
    "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
    "FROM MonitoredFiles "
    "WHERE FileID = ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return std::nullopt;
    }

    if (sqlite3_bind_int(stmt, 1, file_id) != SQLITE_OK) {
        syslog(LOG_ERR, "get_by_id bind failed: %s", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return std::nullopt;
    }

    std::optional<MonitoredFile> result;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);

        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.path = path ? path : "";

        const char* for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.for_users = for_users ? for_users : "";

        f.algorithm = sqlite3_column_int(stmt, 3);

        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        f.baseline_hash = hash ? hash : "";

        result = std::move(f);
    } else if (rc != SQLITE_DONE) {
        syslog(LOG_ERR, "get_by_id step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    return result;
}

std::unordered_map<int, MonitoredFile> MonitoredFileDAO::get_files_by_ids_for_user(
    const std::set<int>& file_ids, int user_id) {

    std::unordered_map<int, MonitoredFile> result;

    if (!db_) {
        syslog(LOG_ERR, "get_files_by_ids_for_user: db_ is null");
        return result;
    }

    if (file_ids.empty()) {
        return result;
    }

    // Проверка на лимит переменных SQLite (обычно 999 или 32766)
    if (file_ids.size() > static_cast<size_t>(sqlite3_limit(db_, SQLITE_LIMIT_VARIABLE_NUMBER, -1) - 1)) {
        syslog(LOG_ERR, "get_files_by_ids_for_user: too many file_ids, exceeds SQLITE_LIMIT_VARIABLE_NUMBER");
        return result;
    }

    if (sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_files_by_ids_for_user: BEGIN TRANSACTION failed: %s", sqlite3_errmsg(db_));
        return result;
    }

    std::string sql = "SELECT FileID, FilePath, ForUsers, HashAlgorithm, Hash "
    "FROM MonitoredFiles "
    "WHERE FileID IN (";

    for (size_t i = 0; i < file_ids.size(); ++i) {
        if (i > 0) sql += ",";
        sql += "?";
    }

    sql += ") AND (ForUsers = '0' OR ForUsers LIKE ?)";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        syslog(LOG_ERR, "get_files_by_ids_for_user prepare failed: %s", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return result;
    }

    int index = 1;
    for (int file_id : file_ids) {
        if (sqlite3_bind_int(stmt, index++, file_id) != SQLITE_OK) {
            syslog(LOG_ERR, "get_files_by_ids_for_user bind file_id failed: %s", sqlite3_errmsg(db_));
            sqlite3_finalize(stmt);
            sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
            return result;
        }
    }

    std::string pattern = "%," + std::to_string(user_id) + ",%";
    if (sqlite3_bind_text(stmt, index, pattern.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        syslog(LOG_ERR, "get_files_by_ids_for_user bind pattern failed: %s", sqlite3_errmsg(db_));
        sqlite3_finalize(stmt);
        sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr);
        return result;
    }

    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        MonitoredFile f;
        f.file_id = sqlite3_column_int(stmt, 0);

        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        f.path = path ? path : "";

        const char* for_users = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        f.for_users = for_users ? for_users : "";

        f.algorithm = sqlite3_column_int(stmt, 3);

        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        f.baseline_hash = hash ? hash : "";

        result[f.file_id] = std::move(f);
    }

    if (rc != SQLITE_DONE) {
        syslog(LOG_ERR, "get_files_by_ids_for_user step failed: %s", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT TRANSACTION", nullptr, nullptr, nullptr);
    return result;
    }
