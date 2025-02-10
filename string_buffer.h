#ifndef _STRING_BUFFER_H_
#define _STRING_BUFFER_H_

typedef struct {
    char* buf;
    int len;
    int cap;
} string_buffer_t;

string_buffer_t* create_string_buffer(const char* str);
void append_string_buffer_char(string_buffer_t*, int);
void append_string_buffer_str(string_buffer_t*, const char*);
void append_string_buffer_fmt(string_buffer_t*, const char*, ...);
void cat_string_buffer(string_buffer_t*, string_buffer_t*);
void clear_string_buffer(string_buffer_t*);

int cmp_string_buffer(string_buffer_t*, string_buffer_t*);
int cmp_string_buffer_str(string_buffer_t*, const char*);

const char* raw_string_buffer(string_buffer_t*);
int len_string_buffer(string_buffer_t*);

#endif /* _STRING_BUFFER_H_ */
