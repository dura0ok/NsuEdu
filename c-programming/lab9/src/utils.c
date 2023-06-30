#include "utils.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"


char *replace_substrings(const char *s1, const char *s2, const char *s3){
    if (!s1 || !s2 || !s3)
        return 0;
    size_t s1_len = strlen(s1);
    if (!s1_len)
        return (char *)s1;
    size_t s2_len = strlen(s2);
    if (!s2_len)
        return (char *)s1;

    size_t count = 0;
    const char *p = s1;
    do {
        p = strstr(p, s2);
        if (p) {
            p += s2_len;
            ++count;
        }
    } while (p);

    if (!count)
        return (char *)s1;

    size_t s1_without_s2_len = s1_len - count * s2_len;
    size_t s3_len = strlen(s3);
    size_t s1_with_s3_len = s1_without_s2_len + count * s3_len;
    if (s3_len &&
        ((s1_with_s3_len <= s1_without_s2_len) || (s1_with_s3_len + 1 == 0)))
        return 0;

    char *s1_with_s3 = (char *)malloc(s1_with_s3_len + 1);
    if (!s1_with_s3)
        return 0;

    char *dst = s1_with_s3;
    const char *start_substr = s1;
    size_t i;
    for (i = 0; i != count; ++i) {
        const char *end_substr = strstr(start_substr, s2);
        size_t substr_len = end_substr - start_substr;
        memcpy(dst, start_substr, substr_len);
        dst += substr_len;
        memcpy(dst, s3, s3_len);
        dst += s3_len;
        start_substr = end_substr + s2_len;
    }


    size_t remains = s1_len - (start_substr - s1) + 1;
    memcpy(dst, start_substr, remains);
    return s1_with_s3;
}

char *read_file_content_to_buffer(char *template_path){
    FILE *template_file;
    char *buffer;

    template_file = fopen (template_path , "rb");
    if( !template_file ){
        return 0;
    }

    fseek(template_file , 0L , SEEK_END);
    long lSize = ftell(template_file);
    rewind(template_file );
    buffer = calloc(sizeof(char), lSize+1);

    if( !buffer ){
        fclose(template_file);
        return NULL;
    }

    if( 1!=fread(buffer , lSize, 1 , template_file) ){
        fclose(template_file);
        free(buffer);
        return NULL;
    }

    fclose(template_file);
    return buffer;
}


