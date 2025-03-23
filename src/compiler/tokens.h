#ifndef _TOKENS_
#define _TOKENS_

#include "value.h"

typedef struct {
    const char* raw; // raw scanned text
    int type;        // token that was scanned
    int line_no;
    int col_no;
    const char* fname;
    value_t* value;
    /*
    union { // processed information, if any.
        int64_t integer_lit;
        double float_lit;
        const char* text;
        void* other;
    } val;
     */
} token_t;

token_t* create_token(const char* text, int type);

#endif /* _TOKENS_ */
