#pragma once
#include "monitored_file.h"
#include <string>
#include <vector>

class Communication_module {
private:
    bool authenticate(int client_fd, const std::string &token);
    void handle_command(int client_fd, const std::string &cmd);
public:
    void start();
    void send_file_list(int fd, const std::vector<Monitored_file> &files);
    void notify_change(int fd, int file_id, time_t timestamp, const std::string &new_hash);
}
