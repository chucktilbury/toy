

#ifndef _COMMON_ERRORS_
#define _COMMON_ERRORS_

#include <stdio.h>
#include <stdlib.h>

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

#endif /* _COMMON_ERRORS_ */
