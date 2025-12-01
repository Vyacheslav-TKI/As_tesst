#include "file_watcher.h"
#include "integrity_checker.h"
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <cstring>
#include <stdexcept>

File_watcher::File_watcher(IntegrityChecker* hasher) : hasher_(hasher) {
    inotify_fd_ = inotify_init1(IN_CLOEXEC);
    if (inotify_fd_ == -1) {
        throw std::runtime_error("inotify_init1 failed");
    }
}

File_watcher::~File_watcher() {
    if (inotify_fd_ >= 0) close(inotify_fd_);
}

void File_watcher::add_watch(const std::string& filepath, int file_id) {
    struct stat st;
    if (stat(filepath.c_str(), &st) != 0) {
        syslog(LOG_WARNING, "File not found: %s", filepath.c_str());
        return;
    }
    if (!S_ISREG(st.st_mode)) {
        syslog(LOG_WARNING, "Not a regular file: %s", filepath.c_str());
        return;
    }

    int wd = inotify_add_watch(inotify_fd_, filepath.c_str(), IN_MODIFY | IN_ATTRIB | IN_DELETE_SELF);
    if (wd == -1) {
        syslog(LOG_ERR, "inotify_add_watch failed for %s: %m", filepath.c_str());
        return;
    }

    wd_to_path_[wd] = filepath;
    path_to_id_[filepath] = file_id;
}

void File_watcher::setFiles(const std::vector<MonitoredFile>& files) {
    // Очищаем старые наблюдения
    for (auto& [wd, _] : wd_to_path_) {
        inotify_rm_watch(inotify_fd_, wd);
    }
    wd_to_path_.clear();
    path_to_id_.clear();

    files_ = files;
    for (const auto& file : files) {
        add_watch(file.path, file.file_id);
    }
}

void File_watcher::process_inotify_events() {
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    ssize_t num_bytes = read(inotify_fd_, buf, sizeof(buf));

    if (num_bytes <= 0) return;

    for (char* ptr = buf; ptr != buf + num_bytes; ) {
        auto* event = reinterpret_cast<struct inotify_event*>(ptr);
        if (event->len && (event->mask & (IN_MODIFY | IN_ATTRIB))) {
            std::string path = wd_to_path_[event->wd];
            auto it = path_to_id_.find(path);
            if (it != path_to_id_.end()) {
                std::string new_hash = hasher_->compute_hash(path, /*algorithm*/ 0); // TODO: передавать алгоритм
                on_file_changed(it->second, new_hash);
            }
        }
        if (event->mask & IN_DELETE_SELF) {
            // файл удалён — уведомить, возможно, удалить из БД
        }
        ptr += sizeof(struct inotify_event) + event->len;
    }
}

void File_watcher::on_file_changed(int file_id, const std::string& new_hash) {
   /* // 1. Логируем событие
    history_dao_->log_change(file_id);

    // 2. Получаем путь файла (из кэша или БД)
    std::string path = get_path_by_file_id(file_id); // реализуй через FileDAO или кэш

    // 3. Уведомляем всех подключённых клиентов через CommunicationModule
    // Для этого нужно передать указатель на comm_ в FileWatcher
    if (comm_) {
        comm_->notify_file_changed(file_id, path, new_hash);
    }*/
}

 void File_watcher::set_hasher(IntegrityChecker* hasher) {
     hasher_ = hasher;
}
