#pragma once

#include "server.h"
#include "user.h"

#include <string>

#include <boost/asio.hpp>


using boost::asio::ip::tcp;


class Connection : public std::enable_shared_from_this <Connection> {
 private:
    std::shared_ptr <tcp::socket> socket;
    boost::asio::streambuf buffer;
    Server* server;
    User* user;

    const std::string signin_message = "Hello, write your login please.\n";

    void read_request();

 public:
    explicit Connection(std::shared_ptr <tcp::socket> socket_, Server* server_): socket(std::move(socket_)), server(server_) {
        user = new User("Anonimous");
    }

    void start();

    void write(const std::string& message);

    User get_user();
};
