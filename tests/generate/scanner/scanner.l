/**
 *
 * @file scanner.l
 *
 * @brief Scanner template.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */

%{
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "errors.h"
#include "tokens.h"
#include "alloc.h"

int yycolumn = 1;

typedef struct _file_info_t_ {
    const char* fname;
    FILE* fp;
    size_t line_no;
    size_t col_no;
    YY_BUFFER_STATE buffer;
    struct _file_info_t_* next;
} file_info_t;

static file_info_t* file_stack = NULL;
static file_info_t* end_file = NULL;

#define YY_USER_ACTION \
  file_stack->col_no = yycolumn; \
  file_stack->line_no = yylineno; \
  if (yylineno == prev_yylineno) \
    yycolumn += yyleng; \
  else { \
    for (yycolumn = 1; yytext[yyleng - yycolumn] != '\n'; ++yycolumn) {} \
    prev_yylineno = yylineno; \
  }

%}


%option yylineno
%option noinput
%option nounput
%option noyywrap
%option header-file="scanner.h"
%option outfile="scanner.c"

%%

   // int start_line, start_column;
   int prev_yylineno = yylineno;



<<EOF>> {

    if(file_stack == NULL) {
        yyterminate();
        return 0;
    }

    file_info_t* fs = file_stack;
    yy_delete_buffer(fs->buffer);
    file_stack = file_stack->next;

    fclose(fs->fp);

    if(file_stack != NULL) {
        yy_switch_to_buffer(file_stack->buffer);
        _FREE(fs->fname);
        _FREE(fs);
    }
    else {
        end_file = fs;
        yyterminate();
    }
}

%%

void open_file(const char* fn) {

    const char *fname = fn; //find_file(fn);
    yyin = fopen(fname, "r");
    if(yyin == NULL)
        FATAL("cannot open input file: \"%s\": %s\n", fname, strerror(errno));

    file_info_t* fs = _ALLOC_TYPE(file_info_t);
    fs->fname = fname;
    fs->next = NULL;
    fs->line_no = 1;
    fs->col_no = 1;

    fs->fp = yyin;
    fs->buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
    yy_switch_to_buffer(fs->buffer);

    if(file_stack != NULL) {
        fs->next = file_stack;
        file_stack = fs;
    }
    else
        file_stack = fs;

}

int get_line_no(void) {

    if(file_stack != NULL)
        return file_stack->line_no;
    else if(end_file != NULL)
        return end_file->line_no;
    else
        return -1; // no file has ever been open
}

int get_col_no(void) {

    if(file_stack != NULL)
        return file_stack->col_no;
    else if(end_file != NULL)
        return end_file->col_no;
    else
        return -1;
}

const char* get_file_name(void) {

    if(file_stack != NULL)
        return file_stack->fname;
    else if(end_file != NULL)
        return end_file->fname;
    else
        return NULL;
}

