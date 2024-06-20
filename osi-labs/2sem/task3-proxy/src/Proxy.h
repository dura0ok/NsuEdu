#pragma once

#include "ThreadPool.h"

class Proxy {
public:
    explicit Proxy(int port);

    [[noreturn]] void handleConnections(ThreadPool &pool) const;

private:
    const int listenSocket;
    const int maxConnections;
};