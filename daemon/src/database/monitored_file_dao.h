// monitored_file_dao.h
#pragma once
#include "entities.h"
#include <sqlite3.h>
#include <vector>
#include <string>

class MonitoredFileDAO {
private:
    sqlite3* db_;

public:
    explicit MonitoredFileDAO(sqlite3* db) : db_(db) {}

    std::vector<MonitoredFile> get_all();
    std::vector<MonitoredFile> get_for_user(int user_id); // учитывает ForUsers
    bool add_file(const MonitoredFile& file);
    bool update_file(const MonitoredFile& file);
    bool delete_file(int file_id);
    bool path_exists(const std::string& path);
};
