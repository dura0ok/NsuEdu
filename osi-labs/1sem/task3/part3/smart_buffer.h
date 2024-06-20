#pragma once
#include <stdio.h>

struct SmartBuf{
    char *buf;
    size_t capacity;
    size_t length;
};


struct SmartBuf* create_smart_buffer();
void push_char_to_buffer(struct SmartBuf *smart_buf, char c);
void push_char_array_to_buffer(struct SmartBuf *smart_buf, char *str);
void print_smart_buffer(struct SmartBuf *smart_buf);
void clear_smart_buffer(struct SmartBuf *smart_buf);
void free_smart_buffer(struct SmartBuf *smart_buf);
struct SmartBuf* create_smart_buf_from_raw_data(char *raw);