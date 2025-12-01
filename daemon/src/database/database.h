// database.h
#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>
#include <vector>
#include "entities.h"

class Database {
private:
    sqlite3* db_ = nullptr;
    void initialize_tables();

public:
    explicit Database(const std::string& path = "/var/lib/celcon/celcon.db");
    std::vector<MonitoredFile> load_monitored_files();
    ~Database();

    sqlite3* get_handle() const { return db_; }

    // Запрещаем копирование
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
};
