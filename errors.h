

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>
#include "tokens.h"

void fatal_error(const char* fmt, ...);
void syntax_error(token_t* tok, const char* fmt, ...);
void syntax_warning(token_t* tok, const char* fmt, ...);
void misc_error(const char* fmt, ...);
int get_errors(void);
int get_warnings(void);
void increment_errors(void);

#define ASSERT(expr, ...)                                                                                 \
    do {                                                                                                  \
        if(!(expr)) {                                                                                     \
            fprintf(stderr, "%s: %s: %d: assertion failed: (%s): ", __FILE__, __func__, __LINE__, #expr); \
            fprintf(stderr, __VA_ARGS__);                                                                 \
            fputc('\n', stderr);                                                                          \
            abort();                                                                                      \
        }                                                                                                 \
    } while(0)

#define FATAL(...)                                                                  \
    do {                                                                            \
        fprintf(stderr, "fatal error: %s: %s: %d: ", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                               \
        fputc('\n', stderr);                                                        \
        exit(1);                                                                    \
    } while(0)

#endif /* _ERRORS_H_ */
