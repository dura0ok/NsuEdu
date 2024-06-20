#pragma once

#include "../http_parser/urlparser.h"

class URIParser {
public:
    URIParser() = delete;

    static httpparser::UrlParser parseURL(const std::string &uri);

    static std::pair<std::string, std::string> parseHost(const std::string &uri);

    static std::string parsePath(const std::string &uri);
};