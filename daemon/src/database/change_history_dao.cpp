// change_history_dao.cpp
#include "change_history_dao.h"

bool ChangeHistoryDAO::log_change(int file_id) {
    const char* sql = "INSERT INTO ChangeHistory (FileID, Time) VALUES (?, strftime('%s', 'now'))";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, file_id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}
