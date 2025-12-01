// integrity_checker.h
#pragma once
#include "hash_algorithm.h"
#include <memory>
#include <vector>

class IntegrityChecker {
private:
    std::vector<std::unique_ptr<HashAlgorithm>> algorithms_;

public:
    IntegrityChecker();
    std::string compute_hash(const std::string& filepath, int algorithm_id) const;
    int get_output_bits(int algorithm_id) const;
};
