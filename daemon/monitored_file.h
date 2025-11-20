#pragma once

struct Monitored_file {
    int file_id;
    std::string path;
    int algorithm; // 0..4
    std::string baseline_hash;
};
