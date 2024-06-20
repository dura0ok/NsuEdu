#include <iostream>
#include <utility>
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t size) : pool_size(size) {
    workers.reserve(pool_size);
    for (size_t i = 0; i < pool_size; ++i) {
        workers.emplace_back(std::make_unique<ThreadWorker>(cacheStorage));
    }
}

void ThreadPool::AddFDToWorker(int fd) {
    auto res = incrementCurrentThread();
    auto worker = workers[res].get();
    worker->addPipe(fd);
    //std::cout << "Add fd " << fd << std::endl;
}

void ThreadPool::AddClientInfoToWorker(ClientInfo *info) {
    auto res = incrementCurrentThread();
    auto worker = workers[res].get();
    worker->transferInfo(info);
}

size_t ThreadPool::incrementCurrentThread() {
    size_t localCurrentThread;
    size_t newCurrentThread;
    localCurrentThread = current_thread.load(std::memory_order_relaxed);
    do {
        newCurrentThread = (localCurrentThread + 1) % pool_size;
    } while (!current_thread.compare_exchange_strong(localCurrentThread, newCurrentThread,
                                                     std::memory_order_relaxed, std::memory_order_relaxed));
    return localCurrentThread;
}
