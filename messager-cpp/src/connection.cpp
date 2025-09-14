#include "connection.h"
#include "user.h"

#include <mutex>
#include <thread>

#include <string>
#include <iostream>

#include <boost/asio.hpp>

#include <unordered_set>


using boost::asio::ip::tcp;


void Connection::read_request() {
    auto self = shared_from_this();
    boost::asio::async_read_until(*socket, buffer, "\n", [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::istream is(&buffer);
            std::string line;
            std::getline(is, line);

            std::cout << "Received: " << line << std::endl;
            
            server->broadcast(line, self);

            read_request();
        } else {
            std::cerr << "Client disconnected." << std::endl;
        }
    });
}

void Connection::start() {
    read_request();
}

void Connection::write(const std::string& message) {
    auto self = shared_from_this();
    boost::asio::async_write(*socket, boost::asio::buffer(message), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            std::cerr << "Client disconnected." << std::endl; 
        }
    });
}

User Connection::get_user() {
    return *user;
}
