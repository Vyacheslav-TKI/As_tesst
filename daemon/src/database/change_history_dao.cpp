// change_history_dao.cpp
#include "change_history_dao.h"

bool ChangeHistoryDAO::log_change(const ChangeRecord &change) {
    const char* sql = "INSERT OR IGNORE INTO ChangeHistory (FileID, NewHash, Time) VALUES (?, ?, ?,  strftime('%s', 'now'))";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, change.file_id);
    sqlite3_bind_text(stmt, 2, change.new_hash.c_str(), -1, SQLITE_STATIC);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

std::vector<ChangeRecord> ChangeHistoryDAO::get_by_file_id(int file_id) {
    const char *sql = "SELECT ChangeID, FileID, NewHash, Time From ChangeHistory WHERE FileID = ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }
    sqlite3_bind_int(stmt, 1, file_id);
    std::vector<ChangeRecord> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ChangeRecord rec;
        rec.change_id = sqlite3_column_int(stmt, 0);
        rec.file_id = sqlite3_column_int(stmt, 1);
        rec.new_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        rec.timestamp = sqlite3_column_int64(stmt, 3);
    }
    sqlite3_finalize(stmt);
    return result;
}
std::vector<ChangeRecord> ChangeHistoryDAO::get_by_time_range(time_t begin, time_t end) {
    const char *sql = "SELECT ChangeID, FileID, NewHash, Time "
                      "FROM ChangeHistory "
                      "WHERE Time BETWEEN ? AND ? "
                      "ORDER BY Time";

    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // Можно добавить логирование ошибки
        // fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db_));
        return {};
    }

    // Используем int64_t для time_t (безопасно на 32/64 бит)
    rc = sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(begin));
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {};
    }

    rc = sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(end));
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return {};
    }

    std::vector<ChangeRecord> result;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        ChangeRecord rec;
        rec.change_id = sqlite3_column_int(stmt, 0);
        rec.file_id = sqlite3_column_int(stmt, 1);

        // Безопасное получение текстовых полей

        const char* new_hash_text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        if (new_hash_text) {
            rec.new_hash = new_hash_text;
        }

        rec.timestamp = sqlite3_column_int64(stmt, 3);

        result.push_back(rec);
    }

    sqlite3_finalize(stmt);

    // Проверка на ошибки выполнения
    if (rc != SQLITE_DONE) {
        // fprintf(stderr, "SQL error during step: %s\n", sqlite3_errmsg(db_));
        return {}; // или можно вернуть частичный результат?
    }

    return result;
}
// Получить изменения для конкретного файла за период
std::vector<ChangeRecord> ChangeHistoryDAO::get_by_file_and_time_range(int file_id, time_t begin, time_t end) {
    const char *sql = "SELECT ChangeID, FileID, NewHash, Time "
                      "FROM ChangeHistory "
                      "WHERE FileID = ? AND Time BETWEEN ? AND ? "
                      "ORDER BY Time";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_int(stmt, 1, file_id);
    sqlite3_bind_int64(stmt, 2, begin);
    sqlite3_bind_int64(stmt, 3, end);

    std::vector<ChangeRecord> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ChangeRecord rec;
        rec.change_id = sqlite3_column_int(stmt, 0);
        rec.file_id = sqlite3_column_int(stmt, 1);
        rec.new_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        rec.timestamp = sqlite3_column_int64(stmt, 3);
        result.push_back(rec);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Получить последние N изменений
std::vector<ChangeRecord> ChangeHistoryDAO::get_last_changes(int limit) {
    const char *sql = "SELECT ChangeID, FileID, NewHash, Time "
                      "FROM ChangeHistory "
                      "ORDER BY Time DESC "
                      "LIMIT ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }

    sqlite3_bind_int(stmt, 1, limit);

    std::vector<ChangeRecord> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ChangeRecord rec;
        rec.change_id = sqlite3_column_int(stmt, 0);
        rec.file_id = sqlite3_column_int(stmt, 1);
        rec.new_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        rec.timestamp = sqlite3_column_int64(stmt, 3);
        result.push_back(rec);
    }

    sqlite3_finalize(stmt);
    return result;
}
