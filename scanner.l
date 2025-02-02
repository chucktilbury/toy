
%{
#include <string.h>
#include <errno.h>

#include "ast.h"
#include "parser.h"
#include "memory.h"
#include "errors.h"
#include "fileio.h"
#include "tokens.h"

int yycolumn = 1;

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

const char* strip_quotes(const char* str) {

    static char buf[64];
    size_t len = strlen(str) - 2;
    memset(buf, 0, sizeof(buf));
    strncpy(buf, &str[1], (len < sizeof(buf))? len: sizeof(buf)-1);
    return buf;
}

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

"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"try"       { return TRY; }
"except"    { return EXCEPT; }
"raise"     { return RAISE; }
"yield"     { return YIELD; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"return"    { return RETURN; }
"print"     { return PRINT; }
"trace"     { return TRACE; }
"exit"      { return EXIT; }
"import"    { return IMPORT; }
"const"     { return CONST; }
"constant"  { return CONST; }
"iter"      { return ITERATOR; }
"iterator"  { return ITERATOR; }
"for"       { return FOR; }
"in"        { return IN; }

"+" {
    yylval.token = create_token(yytext, ADD_OPER);
    return ADD_OPER;
}

"-" {
    yylval.token = create_token(yytext, SUB_OPER);
    return SUB_OPER;
}

"*" {
    yylval.token = create_token(yytext, MUL_OPER);
    return MUL_OPER;
}

"/" {
    yylval.token = create_token(yytext, DIV_OPER);
    return DIV_OPER;
}

"%" {
    yylval.token = create_token(yytext, MOD_OPER);
    return MOD_OPER;
}

"^" {
    yylval.token = create_token(yytext, POW_OPER);
    return POW_OPER;
}

"=="|"equ" {
    yylval.token = create_token(yytext, EQU_OPER);
    return EQU_OPER;
}

"!="|"neq" {
    yylval.token = create_token(yytext, NEQ_OPER);
    return NEQ_OPER;
}

"<="|"lte" {
    yylval.token = create_token(yytext, LTE_OPER);
    return LTE_OPER;
}

">="|"gte" {
    yylval.token = create_token(yytext, GTE_OPER);
    return GTE_OPER;
}

"<"|"lt" {
    yylval.token = create_token(yytext, LT_OPER);
    return LT_OPER;
}

">"|"gt" {
    yylval.token = create_token(yytext, GT_OPER);
    return GT_OPER;
}

"&"|"and" {
    yylval.token = create_token(yytext, AND_OPER);
    return AND_OPER;
}

"|"|"or" {
    yylval.token = create_token(yytext, OR_OPER);
    return OR_OPER;
}

"!"|"not" {
    yylval.token = create_token(yytext, NOT_OPER);
    return NOT_OPER;
}

"integer"|"int" {
    yylval.token = create_token(yytext, INTEGER);
    return INTEGER;
}

"float" {
    yylval.token = create_token(yytext, FLOAT);
    return FLOAT;
}

"string" {
    yylval.token = create_token(yytext, STRING);
    return STRING;
}

"list" {
    yylval.token = create_token(yytext, LIST);
    return LIST;
}

"hash" {
    yylval.token = create_token(yytext, HASH);
    return HASH;
}

"nothing" {
    yylval.token = create_token(yytext, NOTHING);
    return NOTHING;
}

[a-zA-Z_][0-9a-zA-Z_]*  {
    token_t* tok = create_token(yytext, IDENTIFIER);
    tok->val.text = _COPY_STRING(yytext);
    yylval.token = tok;
    return IDENTIFIER;
}

(([1-9][0-9]*)|0)?\.[0-9]*([eE][-+]?[0-9]+)? {
    token_t* tok = create_token(yytext, FLOAT_LIT);
    tok->val.float_lit = strtod(yytext, NULL);
    yylval.token = tok;
    return FLOAT_LIT;
}

([1-9][0-9]*)|0 {
    token_t* tok = create_token(yytext, INTEGER_LIT);
    tok->val.integer_lit = strtol(yytext, NULL, 10);
    yylval.token = tok;
    return INTEGER_LIT;
}

\"[^\"\n]*\" {
    token_t* tok = create_token(yytext, STRING_LIT);
    tok->val.text = _COPY_STRING(strip_quotes(yytext));
    yylval.token = tok;
    return STRING_LIT;
}

\'[^\'\n]*\' {
    token_t* tok = create_token(yytext, STRING_LIT);
    tok->val.text = _COPY_STRING(strip_quotes(yytext));
    yylval.token = tok;
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
    yy_delete_buffer(fs->buffer);
    file_stack = file_stack->next;

    fclose(fs->fp);
    _FREE(fs->fname);
    _FREE(fs);

    if(file_stack != NULL)
        yy_switch_to_buffer(file_stack->buffer);
    else
        yyterminate();
}

%%

void open_file(const char *fname) {

    file_stack_t* fs = _ALLOC_DS(file_stack_t);
    fs->fname = _COPY_STRING(fname);
    fs->next = NULL;
    fs->line = 1;
    fs->col = 1;

    yyin = fopen(fname, "r");
    if(yyin == NULL)
        FATAL("cannot open input file: \"%s\": %s\n", fname, strerror(errno));

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

