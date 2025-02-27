#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errors.h"
#include "tokens.h"

static int errors   = 0;
static int warnings = 0;

void fatal_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "fatal error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);

    exit(1);
}

void syntax_error(token_t* tok, const char* fmt, ...) {

    va_list args;

    if(tok != NULL)
        fprintf(stderr, "%s:%d:%d ", tok->fname, tok->line_no, tok->col_no);
    fprintf(stderr, "syntax error, ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    errors++;
}

void syntax_warning(token_t* tok, const char* fmt, ...) {

    va_list args;

    if(tok != NULL)
        fprintf(stderr, "%s:%d:%d ", tok->fname, tok->line_no, tok->col_no);
    fprintf(stderr, "warning, ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    warnings++;
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

int get_warnings(void) {
    return warnings;
}

void increment_errors(void) {

    errors++;
}
