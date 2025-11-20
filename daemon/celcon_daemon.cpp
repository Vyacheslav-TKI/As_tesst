#include "celcon_daemon.h"
#include "communication_module.h"
#include "file_watcher.h"
#include "integrity_checker.h"
#include "database.h"
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>

Celcon_daemon::Celcon_daemon()
    : comm_(std::make_unique<Communication_module>()),
      watcher_(std::make_unique<File_watcher>()),
      hasher_(std::make_unique<Integrity_checker>()),
      db_(std::make_unique<Database>())
      {}

Celcon_daemon::~Celcon_daemon() = default;

void Celcon_daemon::daemonize() {
    int pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "fork() failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        std::cout << "Celcon daemon started at PID = " << pid;
        exit(EXIT_SUCCESS);
    }
    // Далее всё - дочерний процесс
    if (setsid() < 0) {
        syslog(LOG_ERR, "setsid() failed");
        exit(EXIT_FAILURE);
    }

    chdir("/");
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    struct sigaction sa {};
    sa.sa_handler = [](int) {}
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, nullptr);

    openlog("Celcon_daemon", LOG_PID, LOG_DAEMON);
    mainloop();
    closelog();
}

void Celcon_daemon::mainloop() {
    comm_->start();
    watcher_->start();
    while(true) {
        sleep(1);
    }
}

void Celcon_daemon::exec() {
    daemonize();
}
