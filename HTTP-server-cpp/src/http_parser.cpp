#include <../external/json/include/nlohmann/json.hpp>

#include "cache.h"
#include "logger.h"
#include "http_parser.h"

using json = nlohmann::json;

std::string HttpParser::handle_request(const std::string& raw) {
    std::istringstream stream(raw);
    std::string method, path, version;
    stream >> method >> path >> version;

    Logger::debug("The request");

    if (method == "GET") {
        auto cached = Cache::get(path);
        if (cached != "Not Founded") return cached;

        Logger::debug("The cache wasn't found");

        std::string body = "Hello from GET " + path;
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " +
                               std::to_string(body.size()) +
                               "\r\nContent-Type: text/plain\r\n"
                               "Connection: close\r\n\r\n" + body;
        Cache::put(path, response);

        return response;
    }
    if (method == "POST") {
        size_t pos = raw.find("\r\n\r\n");
        if (pos == std::string::npos) {
            Logger::error("Bad HTTP Request");
            return "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n"
                   "Connection: close\r\n\r\n";
        }
        std::string body = raw.substr(pos + 4);

        try {
            auto j = json::parse(body);
            std::string reply = "Received JSON with pairs: ";
            for (auto it = j.begin(); it != j.end(); ++it) {
                reply += it.key() + ": " + it.value().dump();
                if (std::next(it) != j.end()) {
                    reply += ", ";
                }
            }
            std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " +
                                   std::to_string(reply.size()) +
                                   "\r\nContent-Type: text/plain\r\n"
                                   "Connection: close\r\n\r\n" + reply;
            return response;
        } catch (...) {
            std::string err = "Invalid JSON";
            Logger::error("Invalid JSON");
            return "HTTP/1.1 400 Bad Request\r\nContent-Length: " +
                   std::to_string(err.size()) +
                   "\r\nContent-Type: text/plain\r\n"
                   "Connection: close\r\n\r\n" + err;
        }
    }

    Logger::error("Not Implemented (the method is not GET or POST)");
    return "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0"
           "Connection: close\r\n\r\n";
}
