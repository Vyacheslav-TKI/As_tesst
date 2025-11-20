#pragma once
#include <memory>

class Communication_module;
class File_watcher;
class Integrity_checker;
class Database;

class Celcon_daemon {
private:
    std::unique_ptr<Communication_module> comm_;
    std::unique_ptr<File_watcher> watcher_;
    std::unique_ptr<Integrity_checker> hasher_;
    std::unique_ptr<Database> db_;
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
