/**
 * @file trace.h
 *
 * @brief Verbosity sets the level for messages. Trace verbosity is either on
 * or off.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @version 0.1
 * @date 2025-03-24
 * @copyright Copyright (c) 2025
 */
#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdio.h>
#include "cmdline.h"

#define INIT_TRACE(fh) init_trace(fh)

#define MSG(n, ...)                                   \
    do {                                              \
        if((n) < get_verbosity())            \
            fprintf(get_trace_handle(), __VA_ARGS__); \
    } while(0)

void init_trace(FILE* fp);

// defined in trace.c
extern int trace_depth;

#define PRINT(...)                                     \
    do {                                               \
        if(peek_trace_state()) { \
            print_indent(__VA_ARGS__);                 \
        }                                              \
    } while(0)

#define TRACE(...)                                     \
    do {                                               \
        if(peek_trace_state()) { \
            print_trace(__VA_ARGS__);                  \
        }                                              \
    } while(0)

#define ENTER                                          \
    do {                                               \
        if(peek_trace_state()) { \
            print_enter(__FILE__, __LINE__, __func__); \
        }                                              \
    } while(0)

#define RETURN(...)                                                   \
    do {                                                              \
        if(peek_trace_state()) {                \
            print_return(__FILE__, __LINE__, __func__, #__VA_ARGS__); \
        }                                                             \
        return __VA_ARGS__;                                           \
    } while(0)

#define SEPARATOR                                      \
    do {                                               \
        if(peek_trace_state()) { \
            for(int i = 0; i < 80; i++)                \
                fputc('-', get_trace_handle());        \
            fputc('\n', get_trace_handle());           \
        }                                              \
    } while(0)

#define TRACE_HEADER               \
    do {                           \
        SEPARATOR;                 \
        PRINT("\t%s\n", __func__); \
        SEPARATOR;                 \
    } while(0)

#define PUSH_TRACE_STATE(n) push_trace_state(n)
#define POP_TRACE_STATE() pop_trace_state()
#define PEEK_TRACE_STATE() peek_trace_state()

void reset_trace_depth(int val);
void push_trace_state(int num);
void pop_trace_state(void);
int peek_trace_state(void);
FILE* get_trace_handle(void);
void print_indent(const char* fmt, ...);
void print_trace(const char* fmt, ...);
void print_enter(const char* file, int line, const char* func);
void print_return(const char* file, int line, const char* func, const char* str);

#endif /* _TRACE_H_ */
