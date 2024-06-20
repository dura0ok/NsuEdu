#pragma once

#include "../http_parser/request.h"
#include "../http_parser/response.h"

class HttpParser {
public:
    static httpparser::Request parseRequest(const std::string &buf);

    static int parseStatusCode(const std::string &buf);

    static bool isStatusCodeReceived(const std::string &buf);

    static bool isHttpRequestComplete(const std::string &buf);
};
