#pragma once

#include <string>
#include <memory>

class Connection;

class User {
 private:
    std::string username;

 public:
    explicit User() {
        // init
    }
    explicit User(std::string username_): username(username_) {
        // init
    }

    std::string get_login();
};