#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <stdint.h>

typedef struct {
    const char* raw;
    int type;
    int line_no;
    int col_no;
    const char* fname;
    union {
        int64_t integer_lit;
        double float_lit;
        const char* text;
        void* other;
    } val;
} token_t;

token_t* create_token(const char* text, int type);

#endif
