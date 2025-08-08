#include <winsock2.h>
#include <ws2tcpip.h>

#include "cache.h"
#include "server.h"
#include "logger.h"
#include "http_parser.h"
#include "thread_pool.h"

void Server::init_socket() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        Logger::critical("WSAStartup failed");
        throw std::runtime_error("WSAStartup failed");
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_fd == INVALID_SOCKET)    {
        WSACleanup();
        Logger::critical("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    auto addr_ = reinterpret_cast<sockaddr*>(&addr);
    if (bind(listen_fd, addr_, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(listen_fd);
        WSACleanup();
        Logger::critical("Bind failed");
        throw std::runtime_error("Bind failed");
    }

    if (listen(listen_fd, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listen_fd);
        WSACleanup();
        Logger::critical("Listen failed");
        throw std::runtime_error("Listen failed");
    }

    Logger::info("The server is ready to work");
}

void Server::event_loop() {
    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);

    int max_fd = listen_fd;

    while (true) {
        read_set = master_set;

        int activity = select(0, &read_set, nullptr, nullptr, nullptr);
        if (activity < 0) continue;

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_set)) {
                if (i == listen_fd) {
                    int client_fd = accept(listen_fd, nullptr, nullptr);
                    if (client_fd != -1) {
                        FD_SET(client_fd, &master_set);
                        if (client_fd > max_fd) max_fd = client_fd;

                        Logger::info("New client connected");
                    } else {
                        Logger::error("accept() failed");
                    }
                } else {
                    int client_fd = i;
                    FD_CLR(client_fd, &master_set);

                    Logger::debug("Handling client");

                    pool.enqueue([this, client_fd]() {
                        handle_client(client_fd);
                    });
                }
            }
        }

        Cache::cleanup();
    }
}

void Server::handle_client(int client_fd) {
    char buffer[4096];
    int received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (received <= 0) {
        closesocket(client_fd);
        Logger::error("The client has not been processed");
        return;
    }

    std::string request(buffer, received);
    auto response = HttpParser::handle_request(request);

    size_t total_sent = 0;
    const char* data = response.c_str();
    size_t length = response.size();

    while (total_sent < length) {
        int sent = send(client_fd, data + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            Logger::error("Failed to send full response");
            break;
        }
        total_sent += sent;
    }

    closesocket(client_fd);
}

Server::Server(int port_) : port(port_), pool(4) {}

void Server::run() {
    init_socket();
    Logger::info("The server runs on port {}", port);
    event_loop();
}
