#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <csignal>
#include "ThreadPool.h"
#include "Proxy.h"

int parseThreadPoolSize(const char *threadpoolArg) {
    try {
        size_t pos;
        int result = std::stoi(threadpoolArg, &pos, 10);

        if (pos != std::strlen(threadpoolArg)) {
            throw std::invalid_argument("Invalid thread-pool size");
        }

        return result;
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid thread-pool size: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::out_of_range &e) {
        std::cerr << "Thread pool size out of range: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

ThreadPool *pool;

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Pass threadpool size" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::signal(SIGPIPE, SIG_IGN);

    int threadPoolSize = parseThreadPoolSize(argv[1]);
    auto threadPool = ThreadPool(threadPoolSize);
    pool = &threadPool;
    auto proxy = Proxy(8080);
    proxy.handleConnections(threadPool);
}
