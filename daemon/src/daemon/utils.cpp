#include "utils.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>

static std::string bytes_to_hex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

static std::string generate_salt(size_t length = 16) {
    std::vector<unsigned char> salt(length);
    if (!RAND_bytes(salt.data(), static_cast<int>(length))) {
        // Fallback: не идеально, но для MVP допустимо
        for (size_t i = 0; i < length; ++i) {
            salt[i] = static_cast<unsigned char>(i);
        }
    }
    return bytes_to_hex(salt.data(), length);
}

std::string hash_password(const std::string& password) {
    std::string salt = generate_salt();
    std::string salted = password + salt;

    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.data()), salted.size(), digest);

    std::string hash = bytes_to_hex(digest, SHA256_DIGEST_LENGTH);
    return salt + "$" + hash; // формат: <salt>$<hash>
}

bool verify_password(const std::string& password, const std::string& stored) {
    size_t pos = stored.find('$');
    if (pos == std::string::npos) return false;
    std::string salt = stored.substr(0, pos);
    std::string expected_hash = stored.substr(pos + 1);

    std::string salted = password + salt;
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(salted.data()), salted.size(), digest);
    std::string actual_hash = bytes_to_hex(digest, SHA256_DIGEST_LENGTH);

    return actual_hash == expected_hash;
}
