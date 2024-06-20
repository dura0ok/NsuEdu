#pragma once

#include <string>
#include <netdb.h>
#include "../http_parser/request.h"

class HostConnector {
public:
    static int connectToTargetHost(const httpparser::Request &req);

private:
    static addrinfo *getAddressInfo(const std::string &scheme, const std::string &hostname);

    static int createConnectionToTargetHost(addrinfo *addrInfo);
};