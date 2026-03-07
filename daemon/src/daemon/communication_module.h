#pragma once
#include "monitored_file.h"
#include "auth_manager.h"
#include "file_watcher.h"
#include "integrity_checker.h"
#include "../database/user_dao.h"
#include "../database/monitored_file_dao.h"
#include "../database/change_history_dao.h"
#include <string>
#include <memory>
#include <vector>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../common/json.hpp"
#include <thread>
#include <atomic>

using json = nlohmann::json;

class Database;
class AuthManager;

class CommunicationModule {
private:
    int server_fd_ = -1;
    SSL_CTX* ctx_ = nullptr;
    SSL* ssl_ = nullptr;
    bool client_connected_ = false;

    Database& db_;
    AuthManager auth_manager_;
    File_watcher& watcher_;
    IntegrityChecker* hasher_;
    UserDAO* user_dao_;
    MonitoredFileDAO* file_dao_;
    ChangeHistoryDAO* history_dao_;

    std::string recv_buffer_;

    std::atomic<bool> background_check_active_{false};
    std::thread background_check_thread_;

    void start_background_integrity_check(const std::vector<MonitoredFile>& files);
    void background_integrity_worker(std::vector<MonitoredFile> files);

    void init_tls();
    void handle_command(const std::string& json_str);
    void send_json(const nlohmann::json& response);
    nlohmann::json process_auth(const nlohmann::json& req);
    nlohmann::json process_sync(const nlohmann::json& req);
    nlohmann::json process_add_files(const nlohmann::json& req);
    nlohmann::json process_add_user(const nlohmann::json& req);
    nlohmann::json process_list_users(const nlohmann::json& req);
    nlohmann::json process_stat(const nlohmann::json& req);
    nlohmann::json process_logout(const nlohmann::json& req);
    nlohmann::json process_delete_user(const json& req);
    // ... другие команды

public:
    explicit CommunicationModule(
        Database &db,
        IntegrityChecker* hasher,
        UserDAO* user_dao,
        MonitoredFileDAO* file_dao,
        ChangeHistoryDAO* history_dao,
        File_watcher& watcher
    );

    ~CommunicationModule() = default;

    void start(); // запускает серверный сокет и TLS
    void handle_incoming(); // вызывается из mainloop
    int get_socket_fd() const { return server_fd_; }

    int get_client_fd() const; // возвращает -1, если нет подключения

    // Для отправки событий извне (например, при изменении файла)
    void notify_file_changed(int file_id, const std::string& path, const std::string& new_hash);
};
