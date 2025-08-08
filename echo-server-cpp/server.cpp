#define _WIN32_WINNT 0x0601 // Windows versions NOLINT(*-reserved-identifier)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>
#include <mutex>

// creates mutex and stream for logging
std::mutex logMutex;
std::ofstream logFile("server.log", std::ios::app); // NOLINT(*-interfaces-global-init)

// function for writing logs
void log(const std::string& messange) {
    std::lock_guard <std::mutex> lock(logMutex);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    logFile << std::ctime(&now) << ": " << messange << std::endl;
    std::cout << messange << std::endl;
}

int main() {
    // initializes WSA
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // creates socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // describes ip and port
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // assigns an ip and port to the socket
    auto addr = reinterpret_cast<sockaddr*>(&serverAddr);
    if (bind(listenSocket, addr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // puts the socket into listening mode
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    log("Server started on port 8080");

    // server works
    while (true) {
        // gets client's socket
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            log("Accept failed");
            break;
        }

        log("[+] New connection accepted");

        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::string receivedData(buffer, bytesReceived);
            log("[*] Received: " + receivedData);

            int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
            if (bytesSent == SOCKET_ERROR) {
                log("Send failed");
            }
        } else if (bytesReceived == 0) {
            log("Connection closing...");
        } else {
            log("Recv failed");
        }

        closesocket(clientSocket);
    }

    // frees up memory
    closesocket(listenSocket);
    WSACleanup();
    logFile.close();

    return 0;
}
