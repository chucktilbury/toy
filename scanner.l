
%{
#include <string.h>
#include <errno.h>

#include "ast.h"
#include "parser.h"
#include "memory.h"
#include "errors.h"
#include "fileio.h"
#include "tokens.h"
#include "string_buffer.h"

int yycolumn = 1;
int inline_depth = 0;
string_buffer_t* strbuf = NULL;

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
%x FSTRING
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
"yield"     { return YIELD; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"return"    { return RETURN; }
"exit"      { return EXIT; }
"import"    { return IMPORT; }
"const"|"constant"  { return CONST; }
"iter"|"iterator"   { return ITERATOR; }
"bool"|"boolean"    { return BOOL; }

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

"nothing" {
    yylval.token = create_token(yytext, NOTHING);
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
        //fprintf(stderr, "scanner returns INLINE: %s\n", raw_string_buffer(strbuf));
        clear_string_buffer(strbuf);
        BEGIN(INITIAL);
        return INLINE;
    }
}

    /* double quoted string supports escapes */
\" {
    if(strbuf == NULL)
        strbuf = create_string_buffer(NULL);
    BEGIN(FSTRING);
}

<FSTRING>\\n { append_string_buffer_char(strbuf, '\n'); }
<FSTRING>\\r { append_string_buffer_char(strbuf, '\r'); }
<FSTRING>\\e { append_string_buffer_char(strbuf, '\x1b'); }
<FSTRING>\\t { append_string_buffer_char(strbuf, '\t'); }
<FSTRING>\\b { append_string_buffer_char(strbuf, '\b'); }
<FSTRING>\\f { append_string_buffer_char(strbuf, '\f'); }
<FSTRING>\\v { append_string_buffer_char(strbuf, '\v'); }
<FSTRING>\\\\ { append_string_buffer_char(strbuf, '\\'); }
<FSTRING>\\\" { append_string_buffer_char(strbuf, '\"'); }
<FSTRING>\\\' { append_string_buffer_char(strbuf, '\''); }
<FSTRING>\\\? { append_string_buffer_char(strbuf, '\?'); }
<FSTRING>\\.  { append_string_buffer_char(strbuf, yytext[1]); }
<FSTRING>\\[0-7]{1,3} { append_string_buffer_char(strbuf, (char)strtol(yytext + 1, 0, 8)); }
<FSTRING>\\[xX][0-9a-fA-F]{1,3} { append_string_buffer_char(strbuf, (char)strtol(yytext + 2, 0, 16)); }
<FSTRING>[^\"\n\\]*  { append_string_buffer_str(strbuf, yytext); }

<FSTRING>\" {
    token_t* tok = create_token(raw_string_buffer(strbuf), STRING_LIT);
    tok->val.text = _COPY_STRING(raw_string_buffer(strbuf));
    //fprintf(stderr, "FSTRING returns: \"%s\"\n", tok->val.text);
    clear_string_buffer(strbuf);
    yylval.token = tok;
    BEGIN(INITIAL);
    return STRING_LIT;
}

    /* single quoted string is an absolute literal */
\'[^\'\n]*\' {
    token_t* tok = create_token(yytext, STRING_LIT);
    tok->val.text = _COPY_STRING(strip_quotes(yytext));
    yylval.token = tok;
    return STRING_LIT;
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

