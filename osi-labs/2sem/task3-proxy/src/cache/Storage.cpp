#include "Storage.h"

CacheElement *Storage::getElement(const std::string &key) {
    pthread_rwlock_rdlock(&dataMapLock);
    auto result = getElementNoBlock(key);
    pthread_rwlock_unlock(&dataMapLock);
    return result;
}

CacheElement* Storage::getElementNoBlock(const std::string &key){
    auto it = dataMap.find(key);
    return (it != dataMap.end()) ? it->second.get() : nullptr;
}

std::pair<bool, CacheElement *> Storage::initElement(const std::string &key) {
    CacheElement *res;
    bool isNew;
    pthread_rwlock_wrlock(&dataMapLock);
    auto it = dataMap.find(key);
    if(it != dataMap.end()){
        res = it->second.get();
        isNew = false;
    }else{
        res = dataMap.emplace(key, std::make_unique<CacheElement>()).first->second.get();
        isNew = true;
    }
    pthread_rwlock_unlock(&dataMapLock);
    return std::make_pair(isNew, res);
}

bool Storage::containsKey(const std::string &key) const {
    pthread_rwlock_rdlock(&dataMapLock);
    bool result = (dataMap.find(key) != dataMap.end());
    pthread_rwlock_unlock(&dataMapLock);
    return result;
}

bool Storage::clearElement(const std::string &key) {
    pthread_rwlock_wrlock(&dataMapLock);
    bool ret = false;
    auto cacheElement = getElementNoBlock(key);
    if (cacheElement->isReadersEmpty() && cacheElement->getStatusCode() != 200 && cacheElement->isFinished()) {
        dataMap.erase(key);
        ret = true;
    }

    pthread_rwlock_unlock(&dataMapLock);
    return ret;
}

bool Storage::clearElementForServer(const std::string &key) {
    pthread_rwlock_wrlock(&dataMapLock);
    bool ret = false;
    auto cacheElement = getElementNoBlock(key);
    if (cacheElement->isReadersEmpty() && cacheElement->getStatusCode() != 200) {
        dataMap.erase(key);
        ret = true;
    }

    pthread_rwlock_unlock(&dataMapLock);
    return ret;
}