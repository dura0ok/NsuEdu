/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */

#ifndef HTTPPARSER_URLPARSER_H
#define HTTPPARSER_URLPARSER_H

#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

namespace httpparser {

    class UrlParser {
    public:
        UrlParser()
                : valid(false) {
        }

        explicit UrlParser(const std::string &url)
                : valid(true) {
            parse(url);
        }

        bool parse(const std::string &str) {
            parsedUrl = Url();
            parse_(str);

            return isValid();
        }

        bool isValid() const {
            return valid;
        }

        std::string scheme() const {
            assert(isValid());
            return parsedUrl.scheme;
        }

        std::string username() const {
            assert(isValid());
            return parsedUrl.username;
        }

        std::string password() const {
            assert(isValid());
            return parsedUrl.password;
        }

        std::string hostname() const {
            assert(isValid());
            return parsedUrl.hostname;
        }

        std::string port() const {
            assert(isValid());
            return parsedUrl.port;
        }

        std::string path() const {
            assert(isValid());
            return parsedUrl.path;
        }

        std::string query() const {
            assert(isValid());
            return parsedUrl.query;
        }

        std::string fragment() const {
            assert(isValid());
            return parsedUrl.fragment;
        }

        uint16_t httpPort() const {
            const uint16_t defaultHttpPort = 80;
            const uint16_t defaultHttpsPort = 443;

            assert(isValid());

            if (parsedUrl.port.empty()) {
                if (scheme() == "https")
                    return defaultHttpsPort;
                else
                    return defaultHttpPort;
            } else {
                return parsedUrl.integerPort;
            }
        }

    private:
        bool isUnreserved(char ch) const {
            if (isalnum(ch))
                return true;

            switch (ch) {
                case '-':
                case '.':
                case '_':
                case '~':
                    return true;
            }

            return false;
        }

        void parse_(const std::string &str) {
            enum {
                Scheme,
                SlashAfterScheme1,
                SlashAfterScheme2,
                UsernameOrHostname,
                Password,
                Hostname,
                IPV6Hostname,
                PortOrPassword,
                Port,
                Path,
                Query,
                Fragment
            } state = Scheme;

            std::string usernameOrHostname;
            std::string portOrPassword;

            valid = true;
            parsedUrl.path = "/";
            parsedUrl.integerPort = 0;

            for (size_t i = 0; i < str.size() && valid; ++i) {
                char ch = str[i];

                switch (state) {
                    case Scheme:
                        if (isalnum(ch) || ch == '+' || ch == '-' || ch == '.') {
                            parsedUrl.scheme += ch;
                        } else if (ch == ':') {
                            state = SlashAfterScheme1;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case SlashAfterScheme1:
                        if (ch == '/') {
                            state = SlashAfterScheme2;
                        } else if (isalnum(ch)) {
                            usernameOrHostname = ch;
                            state = UsernameOrHostname;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case SlashAfterScheme2:
                        if (ch == '/') {
                            state = UsernameOrHostname;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case UsernameOrHostname:
                        if (isUnreserved(ch) || ch == '%') {
                            usernameOrHostname += ch;
                        } else if (ch == ':') {
                            state = PortOrPassword;
                        } else if (ch == '@') {
                            state = Hostname;
                            std::swap(parsedUrl.username, usernameOrHostname);
                        } else if (ch == '/') {
                            state = Path;
                            std::swap(parsedUrl.hostname, usernameOrHostname);
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case Password:
                        if (isalnum(ch) || ch == '%') {
                            parsedUrl.password += ch;
                        } else if (ch == '@') {
                            state = Hostname;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case Hostname:
                        if (ch == '[' && parsedUrl.hostname.empty()) {
                            state = IPV6Hostname;
                        } else if (isUnreserved(ch) || ch == '%') {
                            parsedUrl.hostname += ch;
                        } else if (ch == ':') {
                            state = Port;
                        } else if (ch == '/') {
                            state = Path;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case IPV6Hostname:
                        abort(); // TODO
                    case PortOrPassword:
                        if (isdigit(ch)) {
                            portOrPassword += ch;
                        } else if (ch == '/') {
                            std::swap(parsedUrl.hostname, usernameOrHostname);
                            std::swap(parsedUrl.port, portOrPassword);
                            parsedUrl.integerPort = atoi(parsedUrl.port.c_str());
                            state = Path;
                        } else if (isalnum(ch) || ch == '%') {
                            std::swap(parsedUrl.username, usernameOrHostname);
                            std::swap(parsedUrl.password, portOrPassword);
                            parsedUrl.password += ch;
                            state = Password;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case Port:
                        if (isdigit(ch)) {
                            portOrPassword += ch;
                        } else if (ch == '/') {
                            std::swap(parsedUrl.port, portOrPassword);
                            parsedUrl.integerPort = atoi(parsedUrl.port.c_str());
                            state = Path;
                        } else {
                            valid = false;
                            parsedUrl = Url();
                        }
                        break;
                    case Path:
                        if (ch == '#') {
                            state = Fragment;
                        } else if (ch == '?') {
                            state = Query;
                        } else {
                            parsedUrl.path += ch;
                        }
                        break;
                    case Query:
                        if (ch == '#') {
                            state = Fragment;
                        } else if (ch == '?') {
                            state = Query;
                        } else {
                            parsedUrl.query += ch;
                        }
                        break;
                    case Fragment:
                        parsedUrl.fragment += ch;
                        break;
                }
            }

            assert(portOrPassword.empty());

            if (!usernameOrHostname.empty())
                parsedUrl.hostname = usernameOrHostname;
        }


        bool valid;

        struct Url {
            Url() : integerPort(0) {}

            std::string scheme;
            std::string username;
            std::string password;
            std::string hostname;
            std::string port;
            std::string path;
            std::string query;
            std::string fragment;
            uint16_t integerPort;
        } parsedUrl;
    };

} // namespace httpparser

#endif // HTTPPARSER_URLPARSER_H
