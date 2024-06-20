#include <sys/socket.h>
#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include "Proxy.h"


#define MAX_CONNECTIONS 10

Proxy::Proxy(int port) : listenSocket(socket(AF_INET, SOCK_STREAM, 0)), maxConnections(MAX_CONNECTIONS) {

    int enable = 1;
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }

    if (listenSocket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listenSocket, maxConnections) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

[[noreturn]] void Proxy::handleConnections(ThreadPool &threadPool) const {
    while (true) {
        int client_sock_fd = accept(listenSocket, nullptr, nullptr);
        fcntl(client_sock_fd, F_SETFL, fcntl(client_sock_fd, F_GETFL, 0) | O_NONBLOCK);
        threadPool.AddFDToWorker(client_sock_fd);
        std::cout << "new client accepted and added to fd " << client_sock_fd << std::endl;
    }
}