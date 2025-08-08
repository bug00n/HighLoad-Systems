#pragma once

#include <winsock2.h>

#include "thread_pool.h"

class Server {
 private:
    int port;
    SOCKET listen_fd;
    ThreadPool pool;

    void init_socket();
    void event_loop();
    void handle_client(int client_fd);

 public:
    Server(int port_);
    void run();
};
