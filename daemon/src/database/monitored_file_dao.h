// monitored_file_dao.h
#pragma once
#include "entities.h"
#include <sqlite3.h>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include <set>

class MonitoredFileDAO {
private:
    sqlite3* db_;

public:
    explicit MonitoredFileDAO(sqlite3* db) : db_(db) {}

    std::vector<MonitoredFile> get_all();
    std::vector<MonitoredFile> get_for_user(int user_id); // учитывает ForUsers
    std::optional<MonitoredFile> get_by_id_for_user(int file_id, int user_id);
    std::optional<MonitoredFile> get_by_id(int id);
    std::unordered_map<int, std::string> get_files_by_ids_for_user(const std::set<int>& file_ids, int user_id);
    bool add_file(const MonitoredFile& file);
    bool update_file(const MonitoredFile& file);
    bool delete_file(int file_id);
    bool path_exists(const std::string& path);
};
