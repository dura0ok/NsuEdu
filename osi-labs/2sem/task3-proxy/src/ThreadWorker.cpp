#include <sys/poll.h>
#include <bits/poll.h>
#include <sys/socket.h>
#include <iostream>
#include <algorithm>
#include <csignal>
#include <fcntl.h>
#include <cstring>
#include "ThreadWorker.h"
#include "Config.h"
#include "helpers/HttpParser.h"
#include "helpers/HostConnector.h"

ThreadWorker::ThreadWorker(Storage &newStorage) : storage(newStorage) {
    thread = std::thread([this]() { ThreadWorker::worker(); });
    if (pipe2(addPipeFd, O_NONBLOCK) == -1 || pipe2(transferPipeFd, O_NONBLOCK) == -1) {
        throw std::runtime_error("Error creating pipes");
    }

    fds.push_back({addPipeFd[0], POLLIN, 0});
    fds.push_back({transferPipeFd[0], POLLIN, 0});
}

void ThreadWorker::worker() {
    printf("worker is started\n");
    while (true) {

        std::cout << "Thread ID: " << std::this_thread::get_id() << ", Poll result: " <<  "asdasd || " << " ";
        for (auto &el: fds) {
            std::cout << el.fd << " " << el.events << " " << el.revents << " || ";
        }

        std::cout << std::endl;


        int pollResult = poll(fds.data(), fds.size(), -1);


        if (pollResult == -1) {
            throw std::runtime_error("poll error");
        }

        handlePipeMessages();

        for (ssize_t i = 2; i < static_cast<ssize_t>(fds.size()); i++) {
            auto &pfd = fds[i];
            if(serverSocketsURI.count(pfd.fd)) {
                if ((pfd.revents & POLLIN) == POLLIN && handleReadDataFromServer(pfd)) {
                    eraseFDByIndex(i);
                }
            }else if(handleClientConnection(pfd)){
                eraseFDByIndex(i);
            }

            pfd.revents = 0;
        }
    }
}

ssize_t ThreadWorker::eraseFDByIndex(ssize_t &i) {
    fds.erase(fds.begin() + i);
    --i;
    return i;
}

void ThreadWorker::storeClientConnection(int fd, short int events = POLLIN) {
    struct pollfd pfd{};
    pfd.fd = fd;
    pfd.events = events;
    fds.push_back(pfd);
}


bool ThreadWorker::handleClientConnection(pollfd &pfd) {
    if (pfd.revents & POLLIN) {
        return handleClientInput(pfd);
    }

    if (pfd.revents & POLLOUT) {
        return handleClientReceivingResource(pfd);
    }
    return false;
}

void ThreadWorker::addPipe(int writeEnd) {
    if (write(addPipeFd[1], &writeEnd, sizeof(writeEnd)) == -1) {
        throw std::runtime_error("Error writing to addPipe");
    }
}

void ThreadWorker::transferInfo(ClientInfo *info) {
    if (write(transferPipeFd[1], &info, sizeof(ClientInfo *)) == -1) {
        throw std::runtime_error("Error writing to addPipe");
    }
}

void ThreadWorker::storeInfo(ClientInfo *info) {
    printf("%s %d\n", __func__ , info->fd);
    storeClientConnection(info->fd, POLLOUT);
    auto res = clientInfo.insert(std::make_pair(info->fd, info));
    if(!res.second){
        throw std::runtime_error("map inserted failed");
    }
}

void sendHTTP500Error(int clientSocket) {
    std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";

    if (send(clientSocket, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Failed to send HTTP 500 response to the socket" << std::endl;
    }
}

bool ThreadWorker::handleClientInput(pollfd &pfd) {
    //printf("RECEIVE CLIENT INPUT FUNC()\n");
    if(readClientInput(pfd.fd)){
        return true;
    }

    auto &clientBuf = clientBuffersMap[pfd.fd];

    if (!HttpParser::isHttpRequestComplete(clientBuf)) {
        return false;
    }

    httpparser::Request req;
    try{
        req = HttpParser::parseRequest(clientBuf);
    }catch(...){
        sendHTTP500Error(pfd.fd);
        clientBuffersMap.erase(pfd.fd);
        close(pfd.fd);
        return true;
    }


    clientBuffersMap.erase(pfd.fd);

    auto *info = new ClientInfo();
    info->uri = req.uri;
    info->fd = pfd.fd;
    info->offset = 0;

    auto [res, cacheElement] = storage.initElement(req.uri);
    cacheElement->incrementReadersCount();
    printf("Increment readers %s\n", __func__ );
    if (res) {
        printf("ASDD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        cacheElement->initReader(info);

        auto serverFD = HostConnector::connectToTargetHost(req);

        printf("ADD server socket %d\n", serverFD);
        storeClientConnection(serverFD);
        serverSocketsURI.insert(std::make_pair(serverFD, req.uri));
        return true;
    }

    auto insertedResult = clientInfo.insert(std::make_pair(pfd.fd, info));
    if(!insertedResult.second){
        throw std::runtime_error("map inserted failed");
    }

    pfd.events = POLLOUT;
    return false;
}

bool ThreadWorker::readClientInput(int fd) {
    ssize_t bytesRead = recv(fd, sendBuf, WORKER_BUF_SIZE, 0);
    if (bytesRead < 0) {
        clientBuffersMap.erase(fd);
        close(fd);
        return true;
    }

    auto &clientBuf = clientBuffersMap[fd];

    clientBuf += std::string(sendBuf, bytesRead);
    return false;
}

bool ThreadWorker::handleClientReceivingResource(pollfd &pfd) {
    //printf("RECEIVE CLIENT FUNC()\n");
    auto &info = clientInfo.at(pfd.fd);
    auto *cacheElement = storage.getElement(info->uri);

    do{
        auto size = static_cast<ssize_t>(cacheElement->readData(sendBuf, WORKER_BUF_SIZE, info->offset));

        if (size == 0) {
            return cleanClientInfo(cacheElement, info, cacheElement->isFinishReading(info->offset));
        }

        ssize_t bytesSend = send(pfd.fd, sendBuf, size, 0);

        if(bytesSend == -1){
            if(errno == EAGAIN){
                bytesSend = 0;
            }else{
                fprintf(stderr, "ERROR in %s %s\n", __func__, strerror(errno));
                return cleanClientInfo(cacheElement, info, true);
            }
        }

        info->offset += static_cast<ssize_t>(bytesSend);

        if(bytesSend < size){
            if(cleanClientInfo(cacheElement, info, false)){
                return true;
            }
            return false;
        }
    } while (!cacheElement->isFinishReading(info->offset));
    printf("RECEIVE FINISH READ TEST!!!!!!!!!!\n");
    return cleanClientInfo(cacheElement, info, true);
}

bool ThreadWorker::cleanClientInfo(CacheElement *cacheElement, ClientInfo *info, bool closeFD) {
    if (closeFD) {
        clientInfo.erase(info->fd);
        printf("Decrement readers %s\n", __func__ );
        cacheElement->decrementReadersCount();
        close(info->fd);
        storage.clearElement(info->uri);

        fprintf(stderr, "CLOSING %d %s\n", info->fd, __func__);
        delete info;
        return true;
    } else {
       if(cacheElement->initReader(info)){
           clientInfo.erase(info->fd);
           return true;
       }
        return false;
    }
}

bool ThreadWorker::handleReadDataFromServer(pollfd &pfd) {
    auto uri = serverSocketsURI.at(pfd.fd);
    auto *cacheElement = storage.getElement(uri);

    if (storage.clearElementForServer(uri)){
        close(pfd.fd);
        serverSocketsURI.erase(pfd.fd);
        return true;
    }

    for(size_t i = 0; i < 5; i++){
        ssize_t bytesRead = recv(pfd.fd, sendBuf, WORKER_BUF_SIZE, 0);
        if (bytesRead < 0) {

            if(errno != EAGAIN){
                fprintf(stderr, "ERROR < 0 %s\n", strerror(errno));
                cacheElement->markFinished();
                cacheElement->makeReadersReadyToWrite();
                close(pfd.fd);
                serverSocketsURI.erase(pfd.fd);
                return true;
            }else{
                cacheElement->makeReadersReadyToWrite();
                return false;
            }

        }


        if (bytesRead == 0) {
            printf("MARK IS FINISHED %zu\n", cacheElement->getDataSize());
            cacheElement->markFinished();
            serverSocketsURI.erase(pfd.fd);
            fprintf(stderr, "CLOSING %d %s\n", pfd.fd, __func__);
            close(pfd.fd);
            cacheElement->makeReadersReadyToWrite();
            return true;
        }
        printf("BYTES READ %zu\n", bytesRead);
        cacheElement->appendData(sendBuf, bytesRead);
        cacheElement->makeReadersReadyToWrite();
    }
    return false;
}

void ThreadWorker::handlePipeMessages() {
    int fd;
    ClientInfo *info;
    if (fds[0].revents & POLLIN) {
        while (read(addPipeFd[0], &fd, sizeof(fd)) != -1) {
            storeClientConnection(fd);
        }
        fds[0].revents = 0;
    }

    if (fds[1].revents & POLLIN) {
        while (read(transferPipeFd[0], &info, sizeof(ClientInfo *)) != -1) {
            storeInfo(info);
        }
        fds[1].revents = 0;
    }
}


