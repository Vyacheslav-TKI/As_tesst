cmake_minimum_required(VERSION 3.14)
project(celcond)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(celcond
    src/main.cpp
    src/daemon/celcon_daemon.cpp
    src/daemon/communication_module.cpp
    src/daemon/file_watcher.cpp
    src/daemon/integrity_checker.cpp
    src/daemon/json_protocol.cpp
    src/daemon/auth_manager.cpp
    src/daemon/utils.cpp                 # ← для hash_password, verify_password
    src/database/database.cpp
    src/database/user_dao.cpp            # ← для login_exists, add_user
    src/database/monitored_file_dao.cpp  # ← для add_file, get_all
    src/database/change_history_dao.cpp
)

target_include_directories(celcond PRIVATE src)

target_link_libraries(celcond
    ${OPENSSL_LIBRARIES}
    sqlite3
    z
    Threads::Threads
)
