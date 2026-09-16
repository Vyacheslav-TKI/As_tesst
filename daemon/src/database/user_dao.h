// user_dao.h
#pragma once
#include "entities.h"
#include <sqlite3.h>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <set>

class UserDAO {
private:
    sqlite3* db_;

public:
    explicit UserDAO(sqlite3* db);

    bool delete_user(const std::string& user_id);
    std::optional<User> get_by_login(const std::string& login);
    std::unordered_map<int, User> get_by_ids(std::set<int> user_ids);
    bool add_user(const User& user);
    std::vector<User> get_all_users();
    bool update_user(const User& user);
    bool delete_user(int user_id);
    bool login_exists(const std::string& login);
    bool has_any_user();
    bool add_default_admin();
};
