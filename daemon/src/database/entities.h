#pragma once
#include <string>
#include <vector>
#include <optional>
#include <ctime>

struct User {
    int id = -1;
    std::string login;
    std::string password_hash;
    int role = 0; // 0 – уровень 2, 1 – уровень 1, 2 – админ
    std::string fio;
    std::string post;
};

struct MonitoredFile {
    int file_id = -1;
    std::string path;
    std::string for_users; // строка вроде "0" или "1,2,3"
    int algorithm = 0;     // 0..4
    std::string baseline_hash;
};

struct ChangeRecord {
    int change_id = -1;
    int file_id = -1;
    time_t timestamp = 0;
};
