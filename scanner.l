
%{
#include <string.h>
#include <errno.h>

#include "ast.h"
#include "parser.h"
#include "memory.h"
#include "errors.h"

#define MAX_INCL 16

typedef struct _file_stack_t_ {
    const char* fname;
    FILE* fp;
    int line;
    int col;
    YY_BUFFER_STATE buffer;
    struct _file_stack_t_* next;
} file_stack_t;

file_stack_t* file_stack = NULL;

int yycolumn = 1;

const char* strip_quotes(const char* str) {

    static char buf[64];
    size_t len = strlen(str) - 2;
    memset(buf, 0, sizeof(buf));
    strncpy(buf, &str[1], (len < sizeof(buf))? len: sizeof(buf)-1);
    return buf;
}

/* This is executed before every action.
#define YY_USER_ACTION                                                   \
  file_stack->line = prev_yylineno; file_stack->col = yycolumn;          \
  if (yylineno == prev_yylineno) yycolumn += yyleng;                     \
  else {                                                                 \
    for (yycolumn = 1; yytext[yyleng - yycolumn] != '\n'; ++yycolumn) {} \
    prev_yylineno = yylineno;                                            \
  }
*/

#define YY_USER_ACTION                                                   \
  file_stack->col = yycolumn;          \
  if (yylineno == prev_yylineno) yycolumn += yyleng;                     \
  else {                                                                 \
    for (yycolumn = 1; yytext[yyleng - yycolumn] != '\n'; ++yycolumn) {} \
    prev_yylineno = yylineno;                                            \
  }

%}

%option yylineno
%option noinput
%option noyywrap
%option header-file="scanner.h"
%option outfile="scanner.c"

%%

   // int start_line, start_column;
   int prev_yylineno = yylineno;

"integer"   { yylval.type = INTEGER; return INTEGER; }
"int"       { yylval.type = INTEGER; return INTEGER; }
"float"     { yylval.type = FLOAT; return FLOAT; }
"string"    { yylval.type = STRING; return STRING; }
"list"      { yylval.type = LIST; return LIST; }
"hash"      { yylval.type = HASH; return HASH; }
"nothing"   { yylval.type = NOTHING; return NOTHING; }

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"try"       { return TRY; }
"except"    { return EXCEPT; }
"raise"     { return RAISE; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"return"    { return RETURN; }
"print"     { return PRINT; }
"trace"     { return TRACE; }
"exit"      { return EXIT; }
"import"    { return IMPORT; }
"const"     { return CONST; }
"constant"  { return CONST; }

"=="    { return EQU_OPER; }
"equ"   { return EQU_OPER; }
"!="    { return NEQ_OPER; }
"neq"   { return NEQ_OPER; }
"<="    { return LTE_OPER; }
"lte"   { return LTE_OPER; }
">="    { return GTE_OPER; }
"gte"   { return GTE_OPER; }
"<"     { return LT_OPER; }
"lt"    { return LT_OPER; }
">"     { return GT_OPER; }
"gt"    { return GT_OPER; }
"&"     { return AND_OPER; }
"and"   { return AND_OPER; }
"|"     { return OR_OPER; }
"or"    { return OR_OPER; }
"!"     { return NOT_OPER; }
"not"   { return NOT_OPER; }

[a-zA-Z_][0-9a-zA-Z_]*  {
    yylval.text = _COPY_STRING(yytext);
    return IDENTIFIER;
}

(([1-9][0-9]*)|0)?\.[0-9]*([eE][-+]?[0-9]+)? {
    yylval.float_lit = strtod(yytext, NULL);
    return FLOAT_LIT;
}

([1-9][0-9]*)|0 {
    yylval.integer_lit = strtol(yytext, NULL, 10);
    return INTEGER_LIT;
}

\"[^\"\n]*\" {
    yylval.text = _COPY_STRING(strip_quotes(yytext));
    return STRING_LIT;
}

\'[^\'\n]*\' {
    yylval.text = _COPY_STRING(strip_quotes(yytext));
    return STRING_LIT;
}

#[^\n]*\n  {
    /* comment */
    file_stack->line++;
}

;[^\n]*\n  {
    /* comment */
    file_stack->line++;
}

\n {
    file_stack->line++;
}

[ \t\r]+ { /* ignore spaces */ }

. { return yytext[0]; }

<<EOF>> {

    file_stack_t* fs = file_stack;
    file_stack = file_stack->next;

    fclose(fs->fp);
    _FREE(fs->fname);
    _FREE(fs);

    if(file_stack == NULL) {
        yyterminate();
    }
    else {
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(file_stack->buffer);
    }
}

%%

void open_file(const char *fname) {

    file_stack_t* fs = _ALLOC_DS(file_stack_t);
    fs->fname = _COPY_STRING(fname);
    fs->next = NULL;
    fs->line = 1;
    fs->col = 1;

    yyin = fopen(fname, "r");
    if(yyin == NULL) {
        fatal_error("cannot open input file: \"%s\": %s\n", fname, strerror(errno));
        exit(1);
    }
    fs->fp = yyin;
    fs->buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
    yy_switch_to_buffer(fs->buffer);

    if(file_stack != NULL) {
        fs->next = file_stack;
        file_stack = fs;
    }
    else
        file_stack = fs;

    BEGIN(INITIAL);
}

int get_line_no(void) {

    if(file_stack != NULL)
        return file_stack->line;
    else
        return -1;
}

int get_col_no(void) {

    if(file_stack != NULL)
        return file_stack->col;
    else
        return -1;
}

const char* get_file_name(void) {

    if(file_stack != NULL)
        return file_stack->fname;
    else
        return "no file open";
}

