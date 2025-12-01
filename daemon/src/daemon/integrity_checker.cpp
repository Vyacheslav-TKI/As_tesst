// integrity_checker.cpp
#include "integrity_checker.h"
#include "hash_algorithms.cpp"
#include <stdexcept>

IntegrityChecker::IntegrityChecker() {
    algorithms_.reserve(5);
    algorithms_.push_back(std::make_unique<Md5HashAlgorithm>());        // 0 — ВКС
    algorithms_.push_back(std::make_unique<Sha1HashAlgorithm>());       // 1 — Уровень-1
    algorithms_.push_back(std::make_unique<Sha256TruncatedHashAlgorithm>()); // 2 — Уровень-2
    algorithms_.push_back(std::make_unique<Sha256FullHashAlgorithm>()); // 3 — Уровень-3
    algorithms_.push_back(std::make_unique<Gost94HashAlgorithm>());     // 4 — ГОСТ
}

std::string IntegrityChecker::compute_hash(const std::string& filepath, int algorithm_id) const {
    if (algorithm_id < 0 || algorithm_id >= static_cast<int>(algorithms_.size())) {
        return "";
    }
    return algorithms_[algorithm_id]->compute(filepath);
}

int IntegrityChecker::get_output_bits(int algorithm_id) const {
    if (algorithm_id < 0 || algorithm_id >= static_cast<int>(algorithms_.size())) {
        return 0;
    }
    return algorithms_[algorithm_id]->output_bits();
}
