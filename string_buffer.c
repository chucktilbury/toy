#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "string_buffer.h"
#include "memory.h"

static inline void append_buffer(string_buffer_t* buf, const char* str, int len) {

    if(len + buf->len >= buf->cap) {
        while(len + buf->len >= buf->cap)
            buf->cap <<= 1;
        buf->buf = _REALLOC_ARRAY(buf->buf, char, buf->cap);
    }

    memcpy(&buf->buf[buf->len], str, len);
    buf->len += len;
    buf->buf[buf->len] = '\0';
}

string_buffer_t* create_string_buffer(const char* str) {

    string_buffer_t* ptr = _ALLOC_DS(string_buffer_t);
    ptr->len = 0;
    ptr->cap = 1<<3;
    ptr->buf = _ALLOC_ARRAY(char, ptr->cap);

    if(str != NULL)
        append_buffer(ptr, str, strlen(str));

    return ptr;
}

void append_string_buffer_char(string_buffer_t* buf, int c) {

    append_buffer(buf, (const char*)&c, 1);
}

void append_string_buffer_str(string_buffer_t* buf, const char* str) {

    append_buffer(buf, str, strlen(str));
}

void append_string_buffer_fmt(string_buffer_t* buf, const char* fmt, ...) {

    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char* buffer = _ALLOC(len+1);

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    append_buffer(buf, buffer, len);
    _FREE(buffer);
}

void cat_string_buffer(string_buffer_t* buf1, string_buffer_t* buf2) {

    append_buffer(buf1, buf2->buf, buf2->len);
}

int cmp_string_buffer(string_buffer_t* buf1, string_buffer_t* buf2) {

    return strcmp(buf1->buf, buf2->buf);
}

int cmp_string_buffer_str(string_buffer_t* buf, const char* str) {

    return strcmp(buf->buf, str);
}

void clear_string_buffer(string_buffer_t* buf) {

    buf->len = 0;
}

const char* raw_string_buffer(string_buffer_t* buf) {

    return buf->buf;
}

int len_string_buffer(string_buffer_t* buf) {

    return buf->len;
}

