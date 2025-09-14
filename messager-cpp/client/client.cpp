#include <thread>
#include <iostream>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;


// CONF DATA
const char* IP = "127.0.0.1";
const int16_t PORT = 8000;


class Client {
 private:
    boost::asio::io_context io;
    tcp::socket socket;
    boost::asio::streambuf buffer;
    std::thread reader_th;

    void read() {
        boost::asio::async_read_until(socket, buffer, "\n",
            [this](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer);
                    std::string line;
                    std::getline(is, line);

                    std::cout << "Server: " << line << std::endl;

                    read();
                }
            }
        );
    }

 public:
    explicit Client(const char* ip, const int16_t port): socket(io) {
        tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(ip, std::to_string(port));

        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server." << std::endl;

        reader_th = std::thread([this](){
            boost::asio::post(io, [this](){
                read();
             });
            io.run();
        });

    }

    ~Client() {
        socket.close();
        io.stop();
        if (reader_th.joinable()) {
            reader_th.join();
        }
    }

    void send_message(const std::string& msg) {
        boost::asio::async_write(socket, boost::asio::buffer(msg + "\n"),
            [](boost::system::error_code ec, std::size_t){
                if (ec) std::cerr << "Send failed: " << ec.message() << std::endl;
            });
    }
};


int main() {
    try {
        Client cli(IP, PORT);

        std::string message;
        while (std::getline(std::cin, message)) {
            cli.send_message(message);
        }
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}