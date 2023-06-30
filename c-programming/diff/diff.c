#include <stdbool.h>
#include "diff.h"
#include "ctype.h"
#include "stdio.h"
#include "smart_buffer.h"
#include <string.h>


#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


size_t get_new_lines_count(char *buf1, char *buf2, size_t n1, size_t n2){
    char *buf;
    if(n1 >= n2){
        buf = buf1;
    }else{
        buf = buf2;
    }

    size_t cnt = 0;
    for (int i = 0; i < MAX(n1, n2); ++i) {
        if(buf[i] == '\n') cnt++;
    }
    return cnt;
}

int is_printable(char c) {
    return isprint(c) || c == '\0' || c == '\n';
}

int find_first_entry(const char *buf){
    for (int i = 0; buf[i] != '\0'; i++) {
        if(buf[i] == '\n') return i;
    }
    return -1;
}

void shift_line(char *buf){
    int index = find_first_entry(buf);
    if(index == -1){
        for (int i = 0; buf[i] != '\0'; ++i) {
            buf[i] = '\0';
        }
    }
    for (int i = 0; buf[i] != '\0'; i++) {
        buf[i] = buf[i + index + 1];
    }
}

void clear_line_buffers(struct SmartBuf *first_sub_buf, struct SmartBuf *second_sub_buf) {
    first_sub_buf->length = 0;
    memset(&first_sub_buf->buf[0], 0, sizeof(first_sub_buf->buf[0]));
    second_sub_buf->length = 0;
    memset(&second_sub_buf->buf[0], 0, sizeof(second_sub_buf->buf[0]));
}


int get_diff_offset(const char *first_file_buffer, const char *second_file_buffer, size_t n) {
    int offset = 0;
    for (int i = 0; i < n; i++) {
        offset++;
        if (first_file_buffer[i] != second_file_buffer[i]) {
            return offset;
        }
    }
    return -1;
}

bool check_buf_is_printable(char buf[BUFFER_SIZE], size_t n){
    for (int i = 0; i < n; ++i) {
        if(!is_printable(buf[i])) return false;
    }
    return true;
}

bool check_file_is_printable(FILE *file){
    char buf[BUFFER_SIZE];
    while (1) {
        size_t n = fread(buf, 1, sizeof(buf), file);
        if (n == 0) break;
        if(!check_buf_is_printable(buf, n)){
            rewind(file);
            return false;
        }
    }
    rewind(file);
    return true;
}


struct T_buffer_info_answer{
    size_t first_buffer_bytes_read;
    size_t second_buffer_bytes_read;
};

bool print_text_diff(struct SmartBuf *str1, struct SmartBuf *str2) {
    size_t n = MAX(str1->length, str2->length);
    if(get_diff_offset(str1->buf, str2->buf, n) < 0) return false;
    print_smart_buffer(str1);
    printf("\n");
    int i = 0;
    while (str1->buf[i] && str2->buf[i]) {
        if (str1->buf[i] == str2->buf[i])
            putchar(' ');
        else
            putchar('+');
        i++;
    }
    if (str1->buf[i])
        while (str1->buf[i]) {
            putchar('^');
            i++;
        }
    else
        while (str2->buf[i]) {
            putchar('v');
            i++;
        }
    putchar('\n');
    print_smart_buffer(str2);
    return true;
}


void print_buf_by_hex(const char *buf, size_t n) {
    for (int i = 0; i < n; ++i) {
        if(i == 7) printf("| ");
        printf("%02x ", (unsigned char)buf[i]);
    }

    for (size_t i = n; i < BUFFER_SIZE; ++i) {
        if(i == 7) printf("| ");
        printf("%02x ", 0);
    }
    printf("\n");
}

void report_binary_diff(char *first_file_buffer, char *second_file_buffer, struct T_buffer_info_answer answer){
    printf("0x%08x", get_diff_offset(first_file_buffer, second_file_buffer, MAX(answer.first_buffer_bytes_read, answer.second_buffer_bytes_read)));
    print_buf_by_hex(first_file_buffer, answer.first_buffer_bytes_read);
    for (int i = 0; i < BUFFER_SIZE; i++) {

        first_file_buffer[i] != second_file_buffer[i] ? printf("++ ") : printf("   ");
    }
    printf("\n");
    printf("0x%08x", get_diff_offset(first_file_buffer, second_file_buffer, MAX(answer.first_buffer_bytes_read, answer.second_buffer_bytes_read)));
    print_buf_by_hex(second_file_buffer, answer.second_buffer_bytes_read);
}



void fill_buffers(char *first_file_buffer, char *second_file_buffer, struct T_buffer_info_answer *answer,
                  struct SmartBuf *first_sub_buf, struct SmartBuf *second_sub_buf) {
    for (int i = 0; i < (*answer).first_buffer_bytes_read; i++) {
        if(first_file_buffer[i] == '\n'){
            push_char_to_buffer(first_sub_buf, '\n');
            break;
        }
        push_char_to_buffer(first_sub_buf, first_file_buffer[i]);
    }

    for (int i = 0; i < (*answer).second_buffer_bytes_read; i++) {
        if(second_file_buffer[i] == '\n' || second_file_buffer[i] == '\0'){
            push_char_to_buffer(second_sub_buf, '\n');
            break;
        }
        push_char_to_buffer(second_sub_buf, second_file_buffer[i]);
    }

    shift_line(first_file_buffer);
    shift_line(second_file_buffer);

}

void report_text_diff(char *first_file_buffer, char *second_file_buffer, struct T_buffer_info_answer answer){
    struct SmartBuf* first_sub_buf = create_smart_buffer();
    struct SmartBuf* second_sub_buf = create_smart_buffer();
    while(1){
        fill_buffers(first_file_buffer, second_file_buffer, &answer, first_sub_buf, second_sub_buf);
        if(print_text_diff(first_sub_buf, second_sub_buf)) break;
        clear_line_buffers(first_sub_buf, second_sub_buf);
    }
    free_smart_buffer(first_sub_buf);
    free_smart_buffer(second_sub_buf);
}


struct T_buffer_info_answer get_buffers(struct T_file first, struct T_file second, char *first_file_buffer, char *second_file_buffer) {
    size_t first_buffer_bytes_read = fread(first_file_buffer, 1, sizeof(first_file_buffer), first.file);
    size_t second_buffer_bytes_read = fread(second_file_buffer, 1, sizeof(second_file_buffer), second.file);
    struct T_buffer_info_answer answer = {.first_buffer_bytes_read = first_buffer_bytes_read, .second_buffer_bytes_read = second_buffer_bytes_read};
    return answer;
}

void compare_files(struct T_file first, struct T_file second) {
    char *first_file_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    char *second_file_buffer = malloc(sizeof(char) * BUFFER_SIZE);
    memset(first_file_buffer, 0, sizeof(first_file_buffer[0]));
    memset(second_file_buffer, 0, sizeof(second_file_buffer[0]));

    size_t lines_count = 0;
    size_t bytes_count = 0;


    while (1){
        struct T_buffer_info_answer answer = get_buffers(first, second, first_file_buffer, second_file_buffer);
        if(answer.first_buffer_bytes_read == 0 && answer.second_buffer_bytes_read == 0) break;
        size_t n = MAX(answer.first_buffer_bytes_read, answer.second_buffer_bytes_read);

        int offset = get_diff_offset(first_file_buffer, second_file_buffer, n);
        if(offset > 0){
            lines_count += get_new_lines_count(first_file_buffer, second_file_buffer, answer.first_buffer_bytes_read, answer.second_buffer_bytes_read);
            if(check_file_is_printable(first.file) && check_file_is_printable(second.file)){
                printf("Discrepancy at byte %zu At line %zu\n", (size_t)(bytes_count + offset), (size_t)lines_count);
                report_text_diff(first_file_buffer, second_file_buffer, answer);
            }else{
                printf("Discrepancy at byte %zu 0x%02zx\n", bytes_count, bytes_count);
                report_binary_diff(first_file_buffer, second_file_buffer, answer);
            }
            break;
        }

        bytes_count += n;
        lines_count += get_new_lines_count(first_file_buffer, second_file_buffer, answer.first_buffer_bytes_read, answer.second_buffer_bytes_read);
    }


    free(first_file_buffer);
    free(second_file_buffer);

}
