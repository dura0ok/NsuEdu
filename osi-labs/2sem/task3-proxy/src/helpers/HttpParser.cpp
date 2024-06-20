#include <string>
#include <iostream>
#include <regex>

#include "../http_parser/httprequestparser.h"
#include "../http_parser/httpresponseparser.h"
#include "HttpParser.h"

const std::regex statusRegex(R"(\s(\d{3})\s)");

bool HttpParser::isHttpRequestComplete(const std::string &buf) {
    size_t found = buf.find("\r\n\r\n");
    return found != std::string::npos;
}

bool HttpParser::isStatusCodeReceived(const std::string &buf) {
    size_t found = buf.find("\r\n");
    return found != std::string::npos;
}

httpparser::Request HttpParser::parseRequest(const std::string &buf) {
    httpparser::Request request;
    httpparser::HttpRequestParser parser;
    auto data = buf.data();
    auto size = buf.size();
    httpparser::HttpRequestParser::ParseResult res = parser.parse(request, data, data + size);
    if (res == httpparser::HttpRequestParser::ParsingCompleted) {
        return request;
    } else {
        std::cerr << "Parsing failed" << std::endl;
        throw std::runtime_error("http parsing failed");
    }
}

int HttpParser::parseStatusCode(const std::string &buf) {
    std::smatch matches;
    if (std::regex_search(buf, matches, statusRegex) && matches.size() > 1) {
        return std::stoi(matches[1].str());
    }
    return -1;
}