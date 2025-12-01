#include "auth_manager.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "utils.h"

std::string AuthManager::generate_session_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    std::stringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }
    return ss.str();
}

std::optional<std::string> AuthManager::authenticate(const std::string& login, const std::string& password) {
    auto user_opt = user_dao_->get_by_login(login);
    if (!user_opt.has_value()) return std::nullopt;

    if (!verify_password(password, user_opt->password_hash)) {
    return std::nullopt;
}
    const User& user = user_opt.value();

    std::string sid = generate_session_id();
    sessions_[sid] =Session{
        .user = user,  // ← копируем весь объект User
        .last_activity = std::chrono::steady_clock::now()
    };
    return sid;
}

std::optional<User> AuthManager::get_user_by_session(const std::string& sid) {
    cleanup_expired();
    auto it = sessions_.find(sid);
    if (it == sessions_.end()) return std::nullopt;
    it->second.last_activity = std::chrono::steady_clock::now();
    return it->second.user;
}

bool AuthManager::validate_session(const std::string& sid, int min_role) {
    auto user_opt = get_user_by_session(sid);
    return user_opt.has_value() && user_opt->role >= min_role;
}

void AuthManager::logout(const std::string& sid) {
    sessions_.erase(sid);
}

void AuthManager::cleanup_expired() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_activity);
        if (age.count() > SESSION_TIMEOUT_SEC) {
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}
