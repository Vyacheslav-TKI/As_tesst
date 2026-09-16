#pragma once
#include <chrono>
#include "../database/entities.h"

struct Session {
    User user; // вместо user_id + role
    std::chrono::steady_clock::time_point last_activity;
};
