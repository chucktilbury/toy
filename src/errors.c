/**
 * @file errors.c
 * 
 * @brief Error and messaging handlers for the system.
 * 
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errors.h"
#include "tokens.h"

static int errors   = 0;
static int warnings = 0;

/**
 * @brief Fatal error aborts the program.
 * 
 * @param fmt 
 * @param ... 
 */
void fatal_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "fatal error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);

    exit(1);
}

/**
 * @brief Syntax error informs the user that the code will not be generated.
 * 
 * @param tok 
 * @param fmt 
 * @param ... 
 */
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

/**
 * @brief Warning informs the user that a problem exists and gives a 
 * corrective action.
 * 
 * @param tok 
 * @param fmt 
 * @param ... 
 */
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

/**
 * @brief Misc errors.
 * 
 * @param fmt 
 * @param ... 
 */
void misc_error(const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    errors++;
}

/**
 * @brief Return the number of errors seen so far.
 * 
 * @return int 
 */
int get_errors(void) {
    return errors;
}

/**
 * @brief Return the number of warnings seen so far.
 * 
 * @return int 
 */
int get_warnings(void) {
    return warnings;
}

/**
 * @brief Bump the error number. The parser does not have access 
 * to the count.
 * 
 */
void increment_errors(void) {

    errors++;
}
