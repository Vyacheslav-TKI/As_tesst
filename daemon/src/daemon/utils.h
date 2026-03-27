#pragma once
#include <string>

std::string hash_password(const std::string& password);
bool verify_password(const std::string& password, const std::string& stored_hash_with_salt);
