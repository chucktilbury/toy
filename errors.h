

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "ast.h"

void fatal_error(const char* fmt, ...);
void parser_syntax_error(const char* fmt, ...);
void node_syntax_error(ast_node_t* node, const char* fmt, ...);
void misc_error(const char* fmt, ...);
int get_errors(void);

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
