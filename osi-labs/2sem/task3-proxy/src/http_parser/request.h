/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */

#ifndef HTTPPARSER_REQUEST_H
#define HTTPPARSER_REQUEST_H

#include <string>
#include <vector>
#include <sstream>
#include "../helpers/URIParser.h"

namespace httpparser {

    struct Request {
        Request()
                : versionMajor(0), versionMinor(0), keepAlive(false) {}

        struct HeaderItem {
            std::string name;
            std::string value;
        };

        std::string method;
        std::string uri;
        int versionMajor;
        int versionMinor;
        std::vector<HeaderItem> headers;
        std::vector<char> content;
        bool keepAlive;

        [[nodiscard]] std::string inspect() const {
            std::stringstream stream;
            stream << method << " " << uri << " HTTP/"
                   << versionMajor << "." << versionMinor << "\n";

            for (const auto &header: headers) {
                stream << header.name << ": " << header.value << "\n";
            }

            std::string data(content.begin(), content.end());
            stream << data << "\n";
            stream << "+ keep-alive: " << keepAlive << "\n";;
            return stream.str();
        }

        [[nodiscard]] std::string generateHttpQuery() const {
            std::stringstream stream;
            auto path = URIParser::parsePath(uri);
            stream << method << " " << path << " HTTP/1.0\r\n";
            for (const auto &header: headers) {
                if (header.name != "Connection" && header.name != "Proxy-Connection" && header.name != "User-Agent") {
                    stream << header.name << ": " << header.value << "\r\n";
                }
            }
            stream << "\r\n";

            if (headers.empty()) {
                stream << "\r\n";
            }

            auto res = stream.str();
            return res;
        }
    };

} // namespace httpparser


#endif // HTTPPARSER_REQUEST_H
