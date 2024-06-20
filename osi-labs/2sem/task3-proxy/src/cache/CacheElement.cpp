#include "CacheElement.h"
#include "../ThreadPool.h"
#include <cstring>

bool CacheElement::isFinishReading(ssize_t offset) {
    pthread_rwlock_rdlock(&mData);
    auto size = static_cast<ssize_t>(data.size());
    auto localFinished = isFinished();
    pthread_rwlock_unlock(&mData);
    return localFinished && (offset >= size);
}

void CacheElement::markFinished() {
    pthread_rwlock_rdlock(&mData);
    const int localStatusCode = HttpParser::parseStatusCode(data);
    pthread_rwlock_unlock(&mData);
    pthread_rwlock_wrlock(&mData);
    finished = true;
    statusCode = localStatusCode;
    pthread_rwlock_unlock(&mData);
}


bool CacheElement::initReader(ClientInfo *info) {
    pthread_rwlock_rdlock(&mData);
    if(finished){
        pthread_rwlock_unlock(&mData);
        return false;
    }

    pthread_mutex_lock(&mUserBufStates);
    userBufStates.insert(std::make_pair(info->fd, info));
    pthread_mutex_unlock(&mUserBufStates);
    pthread_rwlock_unlock(&mData);
    return true;
}

size_t CacheElement::readData(char *buf, size_t buf_size, ssize_t offset) {
    pthread_rwlock_rdlock(&mData);
    auto copySize = (data.size() >= (buf_size + offset)) ? buf_size : (data.size() - offset);
    std::memcpy(buf, data.data() + offset, copySize);
    pthread_rwlock_unlock(&mData);
    return copySize;
}

void CacheElement::appendData(const char *buf, size_t size) {
    pthread_rwlock_wrlock(&mData);
    data.append(std::string_view(buf, size));
    pthread_rwlock_unlock(&mData);
}

void CacheElement::makeReadersReadyToWrite() {
    pthread_mutex_lock(&mUserBufStates);
    for (auto &userBufState: userBufStates) {
        pool->AddClientInfoToWorker(userBufState.second);
    }
    userBufStates.clear();
    pthread_mutex_unlock(&mUserBufStates);
}

int CacheElement::getStatusCode() const {
    return statusCode;
}

bool CacheElement::isFinished() const {
    return finished;
}

