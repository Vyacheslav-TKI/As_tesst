#include "hash_algorithm.h"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstring>
#include <zlib.h> // для crc32

// Вспомогательная функция: бинарный → hex
std::string bytes_to_hex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

// Вспомогательная функция: чтение всего файла
std::vector<unsigned char> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return {};
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

// === MD5 (для ВКС) ===
class Md5HashAlgorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        auto data = read_file(filepath);
        if (data.empty()) return "";
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5(data.data(), data.size(), digest);
        return bytes_to_hex(digest, MD5_DIGEST_LENGTH);
    }
    std::string name() const override { return "MD5 (stub for VKS)"; }
    int output_bits() const override { return 128; }
};

// === SHA-1 (для Уровень-1) ===
class Sha1HashAlgorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        auto data = read_file(filepath);
        if (data.empty()) return "";
        unsigned char digest[SHA_DIGEST_LENGTH];
        SHA1(data.data(), data.size(), digest);
        return bytes_to_hex(digest, SHA_DIGEST_LENGTH);
    }
    std::string name() const override { return "SHA-1 (stub for Level-1)"; }
    int output_bits() const override { return 160; }
};

// === SHA-256 (для Уровень-2: обрежем до 136 бит = 17 байт) ===
class Sha256TruncatedHashAlgorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        auto data = read_file(filepath);
        if (data.empty()) return "";
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256(data.data(), data.size(), digest);
        // 136 бит = 17 байт
        return bytes_to_hex(digest, 17);
    }
    std::string name() const override { return "SHA-256 truncated to 136-bit (stub for Level-2)"; }
    int output_bits() const override { return 136; }
};

// === SHA-256 (полный, для Уровень-3) ===
class Sha256FullHashAlgorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        auto data = read_file(filepath);
        if (data.empty()) return "";
        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256(data.data(), data.size(), digest);
        return bytes_to_hex(digest, SHA256_DIGEST_LENGTH);
    }
    std::string name() const override { return "SHA-256 (stub for Level-3)"; }
    int output_bits() const override { return 256; }
};

// === Заглушка для ГОСТ Р 34.11-94 ===
class Gost94HashAlgorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        // Позже заменим на libgcrypt или OpenSSL engine
        return "gost94_not_implemented_yet";
    }
    std::string name() const override { return "GOST R 34.11-94 (stub)"; }
    int output_bits() const override { return 256; }
};

class Crc32Algorithm : public HashAlgorithm {
public:
    std::string compute(const std::string& filepath) const override {
        auto data = read_file(filepath);
        if (data.empty()) return "";
        uLong crc = crc32(0L, Z_NULL, 0);
        crc = crc32(crc, data.data(), data.size());
        char buf[9];
        snprintf(buf, sizeof(buf), "%08lx", crc);
        return std::string(buf); // 8 hex = 32 бита
    }
    std::string name() const override { return "CRC32 (VKS)"; }
    int output_bits() const override { return 32; }
};
