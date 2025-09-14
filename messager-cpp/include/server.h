#pragma once

#include <string>
#include <iostream>

#include <boost/asio.hpp>

#include <unordered_set>


using boost::asio::ip::tcp;


class Connection; // pre-defenition

class Server {
 private:
    boost::asio::io_context& io;
    tcp::acceptor acceptor;
    std::unordered_set <std::shared_ptr <Connection>> connects;

    void wait_connection();

 public:
    explicit Server(boost::asio::io_context& io, int16_t port): io(io), acceptor(io, tcp::endpoint(tcp::v4(), port)) {
        // init
    }

    void start();

    void broadcast(const std::string& msg, std::shared_ptr <Connection> sender);
};