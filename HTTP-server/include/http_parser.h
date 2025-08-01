#pragma once

#include <string>

class HttpParser {
 public:
    static std::string handle_request(const std::string& raw);
};