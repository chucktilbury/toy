
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "pointer_list.h"
#include "file_io.h"
#include "fileio.h"
#include "alloc.h"
#include "errors.h"
#include "scanner.h"

/**
 * @brief File data structure type
 *
 */
typedef struct _file_t_ {
    string_t* name;
    FILE* fp;
    int line;
    int column;
    bool is_open;
    struct yy_buffer_state* buffer;
    struct _file_t_* next;
} file_t;

static file_t* file_stack = NULL;

/**
 * @brief Open a file for input and push it on the file stack.
 *
 * @param name
 */
void open_file(const char* name, const char* ext) {

    file_t* ptr = _ALLOC_TYPE(file_t);

    const char* fn = find_file(name, ext);
    yyin = fopen(fn, "r");
    if(yyin == NULL)
        FATAL("cannot open input file: %s: %s", fn, strerror(errno));

    ptr->name = create_string(fn);
    ptr->is_open = true;
    ptr->fp = yyin;
    ptr->buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
    yy_switch_to_buffer(ptr->buffer);
    ptr->next = NULL;

    if(file_stack != NULL)
        ptr->next = file_stack;
    file_stack = ptr;
}

/**
 * @brief Pop the current file off of the stack and destroy it.
 *
 */
void close_file(void) {

    file_t* ptr = file_stack;
    if(ptr != NULL) {
        if(ptr->next != NULL) {
            fclose(ptr->fp);
            destroy_string(ptr->name);
            yy_delete_buffer(ptr->buffer);
            file_stack = ptr->next;
            _FREE(ptr);
        }
        else
            ptr->is_open = false;

        if(file_stack->is_open)
            yy_switch_to_buffer(file_stack->buffer);
    }
}

int get_line_no(void) {

    if(file_stack != NULL)
        return file_stack->line;
    else
        return -1;
}

int get_col_no(void) {

    if(file_stack != NULL)
        return file_stack->column;
    else
        return -1;
}

const string_t* get_file_name(void) {

    if(file_stack != NULL)
        return file_stack->name;
    else
        return NULL;
}

// the others are given by scanner.h
extern int yycolno;
extern int prev_lineno;

void update_numbers(void) {

    if(file_stack != NULL) {
        file_stack->column = yycolno;
        file_stack->line = yylineno;
        if(yylineno == prev_lineno)
            yycolno += yyleng;
        else {
            for(yycolno = 1; yytext[yyleng - yycolno] != '\n'; ++yycolno) {
            }
            prev_lineno = yylineno;
        }
    }
}
