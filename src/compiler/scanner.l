
%{
#include <string.h>
#include <errno.h>

#include "common_string_buffer.h"
#include "common_memory.h"
#include "common_errors.h"
#include "ast.h"
#include "parser.h"
#include "fileio.h"
#include "tokens.h"

int yycolumn = 1;
int inline_depth = 0;
string_buffer_t* strbuf = NULL;

extern int parser_line;
extern char* parser_file;

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

%x INLINE_BLOCK
%option yylineno
%option noinput
%option nounput
%option noyywrap

%%

   // int start_line, start_column;
   int prev_yylineno = yylineno;


"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"return"    { return RETURN; }
"start"     { return START; }
"const"|"constant"  { return CONST; }
"bool"|"boolean"    { return BOOL; }

"import" {
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return IMPORT;
}

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
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return INTEGER;
}

"float" {
    yylval.token = create_token(yytext, FLOAT);
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return FLOAT;
}

"string" {
    yylval.token = create_token(yytext, STRING);
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return STRING;
}

"nothing" {
    yylval.token = create_token(yytext, NOTHING);
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return NOTHING;
}

"true" {
    yylval.token = create_token(yytext, TRUE);
    return BOOL_LIT;
}

"false" {
    yylval.token = create_token(yytext, FALSE);
    return BOOL_LIT;
}

"inline"[ \n\r\t]*"{" {
    inline_depth = 0;
    if(strbuf == NULL)
        strbuf = create_string_buffer(NULL);
    BEGIN(INLINE_BLOCK);
}

<INLINE_BLOCK>"{" {
    inline_depth++;
    append_string_buffer_char(strbuf, '{');
}

<INLINE_BLOCK>[^{}\n] {
    append_string_buffer_str(strbuf, yytext);
}

<INLINE_BLOCK>\n {
    append_string_buffer_char(strbuf, '\n');
    file_stack->line++;
}

<INLINE_BLOCK>"}" {
    if(inline_depth > 0) {
        inline_depth--;
        append_string_buffer_char(strbuf, '}');
    }
    else {
        yylval.token = create_token(raw_string_buffer(strbuf), INLINE);
        yylval.token->value = create_value(INLINE);
        yylval.token->value->val.str = yylval.token->raw;
        clear_string_buffer(strbuf);
        parser_line = get_line_no();
        parser_file = (char*)get_file_name();
        BEGIN(INITIAL);
        return INLINE;
    }
}

\"[^\"\n]*\" {
    token_t* tok = create_token(strip_quotes(yytext), STRING_LIT);
    tok->value = create_value(STRING_LIT);
    tok->value->val.str = tok->raw;
    yylval.token = tok;
    return STRING_LIT;
}

\'[^\'\n]*\' {
    token_t* tok = create_token(strip_quotes(yytext), STRING_LIT);
    tok->value = create_value(STRING_LIT);
    tok->value->val.str = tok->raw;
    yylval.token = tok;
    return STRING_LIT;
}

[a-zA-Z_][0-9a-zA-Z_]*  {
    token_t* tok = create_token(yytext, IDENTIFIER);
    tok->value = create_value(IDENTIFIER);
    tok->value->val.str = tok->raw;
    yylval.token = tok;
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    return IDENTIFIER;
}

(([1-9][0-9]*)|0)?\.[0-9]*([eE][-+]?[0-9]+)? {
    token_t* tok = create_token(yytext, FLOAT_LIT);
    tok->value = create_value(FLOAT_LIT);
    tok->value->val.real = strtod(yytext, NULL);
    yylval.token = tok;
    return FLOAT_LIT;
}

([1-9][0-9]*)|0 {
    token_t* tok = create_token(yytext, INTEGER_LIT);
    tok->value = create_value(INTEGER_LIT);
    tok->value->val.integer = strtol(yytext, NULL, 10);
    yylval.token = tok;
    return INTEGER_LIT;
}

#[^\n]*\n  {
    /* comment */
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
    file_stack->line++;
}

;[^\n]*\n  {
    /* comment */
    parser_line = get_line_no();
    parser_file = (char*)get_file_name();
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

extern const char* base_file_name;

void open_file(const char* fn) {

    const char *fname = find_file(fn);
    yyin = fopen(fname, "r");
    if(yyin == NULL)
        FATAL("cannot open input file: \"%s\": %s\n", fname, strerror(errno));

    file_stack_t* fs = _ALLOC_TYPE(file_stack_t);
    fs->fname = fname;
    fs->next = NULL;
    fs->line = 1;
    fs->col = 1;

    fs->fp = yyin;
    fs->buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
    yy_switch_to_buffer(fs->buffer);

    if(file_stack != NULL) {
        fs->next = file_stack;
        file_stack = fs;
    }
    else {
        file_stack = fs;
        set_base_file_name(fs->fname);
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
        return file_stack->col;
    else
        return -1;
}

const char* get_file_name(void) {

    if(file_stack != NULL)
        return file_stack->fname;
    else
        return get_base_file_name();
}

