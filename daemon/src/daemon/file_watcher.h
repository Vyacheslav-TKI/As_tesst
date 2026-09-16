#pragma once
#include "../database/entities.h"
#include "integrity_checker.h"
#include <string>
#include <vector>
#include <unordered_map>


class File_watcher {
private:
    std::vector<MonitoredFile> files_;
    IntegrityChecker* hasher_ = nullptr; // не владеем, только используем
    int inotify_fd_ = -1;
    std::unordered_map<int, std::string> wd_to_path_; // wd → путь
    std::unordered_map<std::string, int> path_to_id_; // путь → file_id

    void add_watch(const std::string& filepath, int file_id);
    void remove_watch(const std::string& filepath);

public:
    File_watcher(IntegrityChecker* hasher);
    ~File_watcher();

    void setFiles(const std::vector<MonitoredFile>& files);
    void set_hasher(IntegrityChecker* hasher);
    void start();
    int get_inotify_fd() const { return inotify_fd_; }
    void process_inotify_events();

    // Вызывается из inotify-обработчика
    void on_file_changed(int file_id, const std::string& new_hash);
};
