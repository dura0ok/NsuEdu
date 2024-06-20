#include <stdexcept>
#include "URIParser.h"

httpparser::UrlParser URIParser::parseURL(const std::string &uri) {
    httpparser::UrlParser parser;
    if (!parser.parse(uri)) {
        throw std::runtime_error("URI is not correct.");
    }
    return parser;
}

std::pair<std::string, std::string> URIParser::parseHost(const std::string &uri) {
    auto parser = parseURL(uri);
    auto port = parser.port();
    if (port.empty()) {
        port = "80";
    }
    return {parser.hostname(), port};
}

std::string URIParser::parsePath(const std::string &uri) {
    auto parser = parseURL(uri);
    return parser.path();
}
