//
// Created by durachok on 12.01.24.
//

#ifndef HTTP_PROXY_V2_CLIENTINFO_H
#define HTTP_PROXY_V2_CLIENTINFO_H


#include <cstdio>
#include <string>

typedef struct {
    int fd;
    ssize_t offset;
    std::string uri;
} ClientInfo;


#endif //HTTP_PROXY_V2_CLIENTINFO_H
