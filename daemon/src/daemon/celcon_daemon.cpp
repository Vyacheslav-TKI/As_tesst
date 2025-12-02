#include "celcon_daemon.h"
#include "communication_module.h"
#include "file_watcher.h"
#include "integrity_checker.h"
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>

#include <iostream>

Celcon_daemon::Celcon_daemon()
    :
      hasher_(std::make_unique<IntegrityChecker>()),
      watcher_(std::make_unique<File_watcher>(hasher_.get())),
      db_(std::make_unique<Database>()),
      user_dao_(std::make_unique<UserDAO>(db_->get_handle())),
      file_dao_(std::make_unique<MonitoredFileDAO>(db_->get_handle())),
      history_dao_(std::make_unique<ChangeHistoryDAO>(db_->get_handle())),
      comm_(std::make_unique<CommunicationModule>(
          *db_.get(),
          hasher_.get(),
          user_dao_.get(),
          file_dao_.get(),
          *watcher_
      ))
{

    // Передай hasher в watcher, если нужно
    watcher_->set_hasher(hasher_.get());

        // Автоматическое создание админа при первом запуске
    if (!user_dao_->has_any_user()) {
        if (!user_dao_->add_default_admin()) {
            syslog(LOG_ERR, "Failed to create default admin user!");
        } else {
            syslog(LOG_INFO, "Default admin user created: login='admin', password='admin1234'");
        }
    }

}

Celcon_daemon::~Celcon_daemon() = default;

void Celcon_daemon::daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "fork() failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        std::cout << "Celcon daemon started at PID = " << pid << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        syslog(LOG_ERR, "setsid() failed");
        exit(EXIT_FAILURE);
    }

    chdir("/");
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Игнорировать SIGCHLD, чтобы не было зомби
    struct sigaction sa {};
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, nullptr);

    openlog("Celcon_daemon", LOG_PID, LOG_DAEMON);
    mainloop();
    closelog();
}

void Celcon_daemon::mainloop() {
    try {
        auto monitored_files = db_->load_monitored_files();
        watcher_->setFiles(monitored_files);
        comm_->start();

        int inotify_fd = watcher_->get_inotify_fd();

        while (true) {
            // Ждём либо нового клиента (server_fd), либо данных от клиента (client_fd), либо inotify
            fd_set readfds;
            FD_ZERO(&readfds);

            int max_fd = inotify_fd + 1;

            int server_fd = comm_->get_socket_fd();
            FD_SET(server_fd, &readfds);
            max_fd = std::max(max_fd, server_fd + 1);

            int client_fd = comm_->get_client_fd();
            if (client_fd >= 0) {
                FD_SET(client_fd, &readfds);
                max_fd = std::max(max_fd, client_fd + 1);
            }

            FD_SET(inotify_fd, &readfds);

            struct timeval timeout = {100, 0}; // 100 секунд
            int activity = select(max_fd, &readfds, nullptr, nullptr, &timeout);
            if (activity < 0) {
                if (errno == EINTR) continue;
                syslog(LOG_ERR, "select() error: %m");
                break;
            }

            if (FD_ISSET(server_fd, &readfds)) {
                // Новое подключение
                comm_->handle_incoming();
            }

            if (client_fd >= 0 && FD_ISSET(client_fd, &readfds)) {
                // Данные от существующего клиента
                comm_->handle_incoming();
            }

            if (FD_ISSET(inotify_fd, &readfds)) {
                watcher_->process_inotify_events();
            }
        }
    } catch (const std::exception& e) {
        syslog(LOG_CRIT, "Fatal error in mainloop: %s", e.what());
    }
}

void Celcon_daemon::exec() {
    daemonize();
}
