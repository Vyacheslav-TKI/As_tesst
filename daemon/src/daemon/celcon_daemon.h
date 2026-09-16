#pragma once
#include <memory>
#include <vector>
#include <string>
#include "database/database.h"
#include "database/user_dao.h"
#include "database/monitored_file_dao.h"
#include "database/change_history_dao.h"
#include "database/session_log_dao.h"
#include "../database/entities.h"

class CommunicationModule;
class File_watcher;
class IntegrityChecker;
class Database;

class Celcon_daemon {
private:
    // Сначала идут зависимости (базовые сервисы)
    std::unique_ptr<Database> db_;
    std::unique_ptr<IntegrityChecker> hasher_;

    // Потом DAO
    std::unique_ptr<UserDAO> user_dao_;
    std::unique_ptr<MonitoredFileDAO> file_dao_;
    std::unique_ptr<ChangeHistoryDAO> history_dao_;
    std::unique_ptr<SessionLogDAO> session_log_dao_;

    // Потом модули, зависящие от них
    std::unique_ptr<File_watcher> watcher_;
    std::unique_ptr<CommunicationModule> comm_;

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
