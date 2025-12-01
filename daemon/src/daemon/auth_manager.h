#pragma once
#include <string>
#include <unordered_map>
#include <chrono>
#include <optional>
#include "../database/database.h"
#include "../database/entities.h"
#include "../database/user_dao.h"

struct Session {
    User user; // вместо user_id + role
    std::chrono::steady_clock::time_point last_activity;
};

class AuthManager {
private:
    UserDAO *user_dao_;
    std::unordered_map<std::string, Session> sessions_; // session_id → Session
    static constexpr int SESSION_TIMEOUT_SEC = 1800; // 30 мин
    std::string generate_session_id();

public:
    explicit AuthManager(UserDAO *user_dao) : user_dao_(user_dao) {}

    std::optional<User> get_user_by_session(const std::string& sid);

    // Аутентификация: возвращает session_id или пусто
    std::optional<std::string> authenticate(const std::string& login, const std::string& password);

    // Проверка сессии и обновление активности
    bool validate_session(const std::string& session_id, int min_role = 0);

    // Отзыв сессии
    void logout(const std::string& session_id);

    // Удаление просроченных сессий
    void cleanup_expired();
};
