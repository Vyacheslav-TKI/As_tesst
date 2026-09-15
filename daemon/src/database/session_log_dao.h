#pragma once

#include "entities.h"
#include <sqlite3.h>

class SessionLogDAO {
private:
    sqlite3 *db_;
public:
    explicit SessionLogDAO(sqlite3 *db) : db_(db) {}

    std::vector<SessionLog> list_all_sessions();
    bool log_session(int user_id);
};
