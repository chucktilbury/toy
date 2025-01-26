#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "fileio.h"

static int errors = 0;

void fatal_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "fatal error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);

    exit(1);
}

void syntax_error(const char* fmt, ...) {

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
