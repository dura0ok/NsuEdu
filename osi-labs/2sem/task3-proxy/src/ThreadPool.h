#pragma once

#include <vector>
#include <sys/poll.h>
#include <cstddef>
#include <mutex>
#include <memory>
#include "ThreadWorker.h"
#include "cache/Storage.h"
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t pool_size);

    void AddFDToWorker(int fd);

    void AddClientInfoToWorker(ClientInfo *info);

private:
    size_t incrementCurrentThread();

    std::vector<std::unique_ptr<ThreadWorker>> workers;
    std::atomic_size_t current_thread = 0;
    size_t pool_size = 0;
    Storage cacheStorage;


};

extern ThreadPool *pool;