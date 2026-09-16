#include "session_log_dao.h"

bool SessionLogDAO::log_session(int user_id) {
    char const *sql = "INSERT INTO SessionLog (UserID, Time) VALUES (?, strftime('%s', 'now'))";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, user_id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

std::vector<SessionLog> SessionLogDAO::list_all_sessions() {
    char const *sql = "SELECT ID, UserID, Time FROM SessionLog";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return {};
    }
    std::vector<SessionLog> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SessionLog sl;
        sl.id = sqlite3_column_int(stmt, 0);
        sl.user_id = sqlite3_column_int(stmt, 1);
        sl.timestamp = sqlite3_column_int64(stmt, 2);
        result.push_back(sl);
    }
    sqlite3_finalize(stmt);
    return result;
}
