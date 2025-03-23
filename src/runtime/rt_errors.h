/**
 * @file rt_errors.h
 *
 * @brief Publish runtime errors.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-19
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#ifndef _RT_ERRORS_
#define _RT_ERRORS_

#include <stdio.h>
#include <stdlib.h>

void rt_error(const char* fmt, ...);
void rt_dump_callstack(void);

#define RT_ASSERT(expr, ...)                                                                                 \
    do {                                                                                                  \
        if(!(expr)) {                                                                                     \
            fprintf(stderr, "runtime assertion failed: (%s): ", #expr); \
            fprintf(stderr, __VA_ARGS__);                                                                 \
            fputc('\n', stderr);                                                                          \
            rt_dump_callstack(); \
            abort();                                                                                      \
        }                                                                                                 \
    } while(0)




#endif /* _RT_ERRORS_ */
