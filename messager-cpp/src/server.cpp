#include "server.h"
#include "connection.h"
#include "user.h"

#include <mutex>
#include <thread>

#include <string>
#include <iostream>

#include <boost/asio.hpp>

#include <unordered_set>


using boost::asio::ip::tcp;


void Server::wait_connection() {
    std::shared_ptr <tcp::socket> socket = std::make_shared <tcp::socket> (io);
    acceptor.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            auto session = std::make_shared <Connection> (socket, this);
            std::cout << "Con" << std::endl;
            connects.insert(session);
            session->start();
        }
        wait_connection();
    });
}

void Server::start () {
    std::cout << "The server has been started on 0.0.0.0:8000" << std::endl;
    wait_connection();
}

void Server::broadcast(const std::string& msg, std::shared_ptr <Connection> sender) {
    for (auto& client : connects) {
        if (client != sender)
            client->write("from " + sender->get_user().get_login() + ": " + msg + '\n');
    }
}
