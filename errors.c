#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "fileio.h"
#include "errors.h"
#include "tokens.h"

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

void token_syntax_error(token_t* tok, const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "%s:%d:%d syntax error, ", tok->fname, tok->line_no, tok->col_no);

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
