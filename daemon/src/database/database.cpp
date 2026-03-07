// database.cpp
#include "database.h"
#include "monitored_file_dao.h"
#include <syslog.h>
#include <stdexcept>
#include <filesystem>

Database::Database(const std::string& path) {
    //std::filesystem::create_directories("/var/lib/celcon");

    int rc = sqlite3_open(path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        syslog(LOG_ERR, "Cannot open database: %s", sqlite3_errmsg(db_));
        throw std::runtime_error("Failed to open SQLite DB");
    }

    initialize_tables();
}

Database::~Database() {
    if (db_) sqlite3_close(db_);
}

void Database::initialize_tables() {
    const char* schema[] = {
        "CREATE TABLE IF NOT EXISTS Users ("
            "UserID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "UserLogin TEXT NOT NULL UNIQUE,"
            "UserPasswordHash TEXT NOT NULL,"
            "UserRole INTEGER NOT NULL CHECK(UserRole BETWEEN 0 AND 2),"
            "FIO TEXT NOT NULL,"
            "Post TEXT NOT NULL"
        ");",

        "CREATE TABLE IF NOT EXISTS MonitoredFiles ("
            "FileID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "FilePath TEXT NOT NULL,"
            "ForUsers TEXT NOT NULL,"
            "HashAlgorithm INTEGER NOT NULL CHECK(HashAlgorithm BETWEEN 0 AND 4),"
            "Hash TEXT NOT NULL"
        ");",

        "CREATE TABLE IF NOT EXISTS ChangeHistory ("
            "ChangeID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "FileID INTEGER NOT NULL,"
            "OldHash TEXT,"
            "NewHash TEXT,"
            "Time INTEGER NOT NULL,"
            "FOREIGN KEY (FileID) REFERENCES MonitoredFiles(FileID) ON DELETE CASCADE"
        ");",

        "CREATE TRIGGER IF NOT EXISTS PreventDuplicateLastHash "
    "BEFORE INSERT ON ChangeHistory "
    "BEGIN "
        "SELECT CASE "
            "WHEN ("
                "SELECT NewHash FROM ChangeHistory "
                "WHERE FileID = NEW.FileID "
                "ORDER BY Time DESC LIMIT 1"
            ") = NEW.NewHash THEN "
                "RAISE(ABORT, 'File has not changed since last check') "
        "END;"
    "END;",
        nullptr
    };

    for (int i = 0; schema[i]; ++i) {
        char* errmsg = nullptr;
        if (sqlite3_exec(db_, schema[i], nullptr, nullptr, &errmsg) != SQLITE_OK) {
            syslog(LOG_ERR, "DB schema error: %s", errmsg);
            sqlite3_free(errmsg);
            throw std::runtime_error("Failed to create tables");
        }
    }
}

std::vector<MonitoredFile> Database::load_monitored_files() {
    MonitoredFileDAO dao(db_);
    return dao.get_all();
}
