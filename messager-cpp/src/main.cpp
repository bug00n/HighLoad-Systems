#include "user.cpp"
#include "connection.cpp"
#include "server.cpp"

#include <iostream>

#include <boost/asio.hpp>


using boost::asio::ip::tcp;


// CONF DATA
const int16_t PORT = 8000;


int main() {
    try {
        boost::asio::io_context io;

        // Change ctrl+C shutdown
        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait([&](const boost::system::error_code&, int signal) {
            std::cout << "\tThe server has been stopped" << std::endl;
            io.stop();
        });

        Server server(io, PORT);

        server.start();
        io.run();

        return EXIT_SUCCESS;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
