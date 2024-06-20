#pragma once

#include <string>
#include <map>
#include <mutex>
#include <vector>
#include <sys/poll.h>
#include <shared_mutex>
#include "../ClientInfo.h"
#include <atomic>

class CacheElement {
public:
    CacheElement() {
        pthread_rwlock_init(&mData, nullptr);
        pthread_mutex_init(&mUserBufStates, nullptr);
    }

    ~CacheElement() {
        pthread_rwlock_destroy(&mData);
        pthread_mutex_destroy(&mUserBufStates);
    }


    bool isFinishReading(ssize_t offset);

    void markFinished();

    bool initReader(ClientInfo *info);


    size_t readData(char *buf, size_t buf_size, ssize_t offset);

    void appendData(const char *buf, size_t size);

    void makeReadersReadyToWrite();

    size_t getDataSize() {
        return data.size();
    }

    void decrementReadersCount() {
        readersCount.fetch_sub(1, std::memory_order_relaxed);
    }

    void incrementReadersCount() {
        readersCount.fetch_add(1, std::memory_order_relaxed);
    }

    bool isReadersEmpty() {
        return readersCount.load(std::memory_order_relaxed) == 0;
    }


    [[nodiscard]] int getStatusCode() const;
    [[nodiscard]] bool isFinished() const;
private:
    std::map<int, ClientInfo *> userBufStates;
    pthread_rwlock_t mData{};
    pthread_mutex_t mUserBufStates = PTHREAD_MUTEX_INITIALIZER;
    std::string data;
    std::atomic_size_t readersCount = 0;
    bool finished{};

private:
    int statusCode = -1;
};
