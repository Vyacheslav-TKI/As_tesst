// user_dao.cpp
#include "user_dao.h"
#include <sqlite3.h>
#include <syslog.h>

std::optional<User> UserDAO::get_by_login(const std::string& login) {
    const char* sql = "SELECT UserID, UserPasswordHash, UserRole, FIO, Post FROM Users WHERE UserLogin = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int(stmt, 0);
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.role = sqlite3_column_int(stmt, 2);
        u.fio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.post = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.login = login;
        sqlite3_finalize(stmt);
        return u;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

bool UserDAO::add_user(const User& user) {
    const char* sql = "INSERT INTO Users (UserLogin, UserPasswordHash, UserRole, FIO, Post) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, user.login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, user.role);
    sqlite3_bind_text(stmt, 4, user.fio.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, user.post.c_str(), -1, SQLITE_STATIC);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool UserDAO::login_exists(const std::string& login) {
    const char* sql = "SELECT 1 FROM Users WHERE UserLogin = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

// остальные методы аналогично (через prepared statements)
