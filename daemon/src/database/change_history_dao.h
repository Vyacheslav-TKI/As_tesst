// change_history_dao.h
#pragma once
#include "entities.h"
#include <sqlite3.h>
#include <vector>
#include <string>
class ChangeHistoryDAO {
    sqlite3* db_;
public:
    explicit ChangeHistoryDAO(sqlite3* db) : db_(db) {}
    bool log_change(const ChangeRecord &change);
    std::vector<ChangeRecord> get_by_file_id(int file_id);
    std::vector<ChangeRecord> get_by_time_range(time_t begin, time_t end);
    std::vector<ChangeRecord> get_last_changes(int limit);
    std::vector<ChangeRecord> get_by_file_and_time_range(int file_id, time_t begin, time_t end);
};
