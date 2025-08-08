#include "server.h"
#include "logger.h"

void on_exit() {
    Logger::info("The server was shut down");
    spdlog::shutdown();
}

int main() {
    #ifdef _WIN32
        #include <windows.h>
        SetConsoleOutputCP(CP_UTF8);
    #endif

    Logger::init("C:/Users/User/HTTP-server/log/server.log", Level::Debug);
    std::atexit(on_exit);

    Server server(8080);
    server.run();

    return 0;
}
