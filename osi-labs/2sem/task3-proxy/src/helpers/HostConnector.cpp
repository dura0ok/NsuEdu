#include <vector>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <csignal>
#include <thread>
#include "HostConnector.h"

int HostConnector::connectToTargetHost(const httpparser::Request &req) {
    auto [host, port] = URIParser::parseHost(req.uri);
    auto *addr = HostConnector::getAddressInfo(host, port);
    auto connectFD = createConnectionToTargetHost(addr);
    auto httpQuery = req.generateHttpQuery();
    send(connectFD, httpQuery.data(), httpQuery.size(), 0);
    return connectFD;
}

addrinfo *HostConnector::getAddressInfo(const std::string &hostname, const std::string &port) {
    struct addrinfo hints{}, *res;
    int attempts = 0;
    const int maxAttempts = 10;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    while (attempts < maxAttempts) {
        if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res) != 0) {
            std::cerr << "Error in resolving host. Retrying..." << std::endl;
            attempts++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            return res;
        }
    }

    throw std::runtime_error("Failed to resolve host after multiple attempts");
}


int HostConnector::createConnectionToTargetHost(addrinfo *addrInfo) {
    int clientSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (clientSocket == -1) {
        freeaddrinfo(addrInfo);
        throw std::runtime_error("Error creating socket");
    }

    if (connect(clientSocket, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
        std::cerr << "Error connecting to server\n";
        close(clientSocket);
        freeaddrinfo(addrInfo);
        throw std::runtime_error("Error connecting to server");
    }

    return clientSocket;
}
