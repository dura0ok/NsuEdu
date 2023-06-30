#include "smart_buffer.h"
#include "malloc.h"


struct SmartBuf* create_smart_buffer(){
    const size_t default_buffer_size = 1;
    struct SmartBuf *new_buf = malloc(sizeof(struct SmartBuf));
    if(new_buf == NULL) return NULL;

    char *data = malloc(sizeof(char) * (default_buffer_size + 1));
    if(data == NULL){
        free(new_buf);
        return NULL;
    }

    new_buf->capacity = default_buffer_size + 1;
    new_buf->length = 0;
    new_buf->buf = data;
    return new_buf;
}

void push_char_to_buffer(struct SmartBuf *smart_buf, char c){
    if(smart_buf->length == smart_buf->capacity - 1){
        smart_buf->buf = realloc(smart_buf->buf, smart_buf->capacity * 2);
        smart_buf->capacity *= 2;
    }

    smart_buf->buf[smart_buf->length] = c;
    smart_buf->buf[smart_buf->length+1] = '\0';
    smart_buf->length++;
}

void push_char_array_to_buffer(struct SmartBuf *smart_buf, char *str){
    char *runner = str;
    while (*runner != '\0'){
        push_char_to_buffer(smart_buf, *runner);
        runner++;
    }

}

void free_smart_buffer(struct SmartBuf *smart_buf){
    if(smart_buf == NULL) return;
    if(smart_buf->buf == NULL){
        free(smart_buf);
        return;
    }
    free(smart_buf->buf);
    free(smart_buf);
}


