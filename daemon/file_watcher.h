#pragma once
#include "Monitored_file.h"

class File_watcher {
private:
    std::vector<Monitored_file> files_;
    std::unique_ptr<Integrity_checker> hasher_;
public:
    void setFiles(const std::vector<Monitored_file> &files);
    void start();
    void on_file_changed(int file_id, const std::string &new_hash);
}
