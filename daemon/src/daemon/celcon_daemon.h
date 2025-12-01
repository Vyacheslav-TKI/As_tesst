#pragma once
#include <memory>
#include "database/database.h"
#include "database/user_dao.h"
#include "database/monitored_file_dao.h"
#include "database/change_history_dao.h"
#include "../database/entities.h"

class CommunicationModule;
class File_watcher;
class IntegrityChecker;
class Database;

class Celcon_daemon {
private:
    std::unique_ptr<CommunicationModule> comm_;
    std::unique_ptr<File_watcher> watcher_;
    std::unique_ptr<IntegrityChecker> hasher_;
    std::unique_ptr<Database> db_;
    std::unique_ptr<UserDAO> user_dao_;
    std::unique_ptr<MonitoredFileDAO> file_dao_;
    std::unique_ptr<ChangeHistoryDAO> history_dao_;
protected:
    void daemonize();
    void mainloop();
    void operate(int fd);
    static void sighandler(int signum);

public:
    Celcon_daemon();
    ~Celcon_daemon();
    void exec();
};
