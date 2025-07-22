#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "alloc.h"
#include "cmdline.h"

static int trace_depth = 0;
static int trace_increment = 2;
static FILE* trace_file_handle = NULL;
static int verbosity = 0;

typedef struct _verbosity_stack_t_ {
    int verbosity;
    struct _verbosity_stack_t_* next;
} verbosity_stack_t;

static verbosity_stack_t* stack;

void push_trace_state(int num) {

    verbosity_stack_t* ptr = _ALLOC_TYPE(verbosity_stack_t);
    ptr->verbosity = num;

    if(stack != NULL)
        ptr->next = ptr;
    stack = ptr;
}

void pop_trace_state(void) {

    if(stack != NULL) {
        verbosity_stack_t* ptr = stack;
        stack = stack->next;
        _FREE(ptr);
    }
}

int peek_trace_state(void) {

    if(stack != NULL)
        return stack->verbosity;
    else
        return 0;
}

void increment_trace_depth(void) {

    trace_depth++;
}

void decrement_trace_depth(void) {

    if(trace_depth > 0)
        trace_depth--;
}

void reset_trace_depth(int val) {

    trace_depth = val;
}

int get_trace_depth(void) {

    return trace_depth;
}

void init_trace(FILE* fp) {

    if(fp == NULL)
        trace_file_handle = stdout;
    else
        trace_file_handle = fp;

    verbosity = (int)strtol(raw_string(get_cmd_opt("verbosity")), NULL, 10);
    push_trace_state(0);
}

FILE* get_trace_handle(void) {

    return trace_file_handle;
}

int get_verbosity(void) {

    return verbosity;
}

void print_indent(const char* fmt, ...) {

    fprintf(trace_file_handle, "%*s", trace_depth * trace_increment, "");
    va_list args;

    va_start(args, fmt);
    vfprintf(trace_file_handle, fmt, args);
    va_end(args);
}

void print_trace(const char* fmt, ...) {

    fprintf(trace_file_handle, "%*s", trace_depth * trace_increment, "");
    fprintf(trace_file_handle, "TRACE: ");
    va_list args;

    va_start(args, fmt);
    vfprintf(trace_file_handle, fmt, args);
    va_end(args);
    fprintf(trace_file_handle, "\n");
}

void print_enter(const char* file, int line, const char* func) {

    fprintf(trace_file_handle, "%*s", trace_depth * trace_increment, "");
    fprintf(trace_file_handle, "ENTER: %s: %d: %s()\n", file, line, func);
    increment_trace_depth();
}

void print_return(const char* file, int line, const char* func, const char* str) {

    decrement_trace_depth();
    fprintf(trace_file_handle, "%*s", trace_depth * trace_increment, "");
    fprintf(trace_file_handle, "RETURN: %s: %d: %s(): %s\n", file, line, func, str);
}
