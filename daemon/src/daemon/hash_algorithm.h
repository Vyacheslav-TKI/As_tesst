// hash_algorithm.h
#pragma once
#include <string>

class HashAlgorithm {
public:
    virtual ~HashAlgorithm() = default;
    virtual std::string compute(const std::string& filepath) const = 0;
    virtual std::string name() const = 0;
    virtual int output_bits() const = 0;
};
