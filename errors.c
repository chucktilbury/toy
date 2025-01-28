#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "fileio.h"
#include "errors.h"
#include "hash.h"

static int errors = 0;

#define FIND_ATTRIB(node, name, ptr) find_hashtable(node->table, #name, &ptr)

#define GET_ATTRIB(node, name, ptr)                                  \
do {                                                             \
    if(!FIND_ATTRIB(node, name, ptr))                            \
        FATAL("improper node format: \"%s\" not found", #name);  \
        else if(ptr == NULL)                                         \
            FATAL("improper node format: \"%s\" is invalid", #name); \
} while(0)

void fatal_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "fatal error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);

    exit(1);
}

void node_syntax_error(ast_node_t* node, const char* fmt, ...) {

    va_list args;
    void* ptr;

    GET_ATTRIB(node, line_no, ptr);
    int line = *(int*)ptr;
    GET_ATTRIB(node, column_no, ptr);
    int col = *(int*)ptr;
    GET_ATTRIB(node, file_name, ptr);
    const char* fname = (const char*)ptr;

    fprintf(stderr, "%s:%d:%d syntax error, ", fname, line, col);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    errors++;
}

void parser_syntax_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "%s:%d:%d ", get_file_name(), get_line_no(), get_col_no());

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    errors++;
}

void misc_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    errors++;
}


int get_errors(void) {
    return errors;
}
