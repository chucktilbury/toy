
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#include "errors.h"
#include "parser.h"
//#include "tokens.h"

// string buffer to get token from
static char strbuf[1024*64];

typedef struct _file_name_stack {
    int line_no;
    char *name;
    struct _file_name_stack *next;
} _file_name_stack;


static char buffer[1024*64];
static int bidx = 0;
static _file_name_stack *name_stack;
int num_errors = 0; // global updated by parser

// these funcs support the string scanner
static void append_char(char ch) {
    if((sizeof(buffer)-1) > bidx) {
        buffer[bidx] = ch;
        bidx++;
    }
    else {
        //scanner_error("buffer overrun");
        printf("buffer overrun");
    }
}

static void append_str(char *str) {
    if((sizeof(buffer)-1) > (strlen(buffer) + strlen(str))) {
        strcat(buffer, str);
        bidx = strlen(buffer);
    }
    else {
        //scanner_error("buffer overrun");
        printf("buffer overrun");
    }
}

%}

    /* state definitions */
%x SQUOTES
%x DQUOTES
%option noinput nounput

%%

    /* overhead */
\n           {name_stack->line_no ++; yylineno++; }
[ \t\r]       {}

    /* keywords */
"nothing"       { strncpy(strbuf, "NOTHING_TOK", sizeof(strbuf)); return NOTHING_TOK; }
"true"          { strncpy(strbuf, "TRUE_TOK", sizeof(strbuf)); return TRUE_TOK; }
"false"         { strncpy(strbuf, "FALSE_TOK", sizeof(strbuf)); return FALSE_TOK; }
"include"       { strncpy(strbuf, "INCLUDE_TOK", sizeof(strbuf)); return INCLUDE_TOK; }
"var"           { strncpy(strbuf, "VAR_TOK", sizeof(strbuf)); return VAR_TOK; }
"class"         { strncpy(strbuf, "CLASS_TOK", sizeof(strbuf)); return CLASS_TOK; }
"method"        { strncpy(strbuf, "METHOD_TOK", sizeof(strbuf)); return METHOD_TOK; }
"create"        { strncpy(strbuf, "CREATE_TOK", sizeof(strbuf)); return CREATE_TOK; }
"destroy"       { strncpy(strbuf, "DESTROY_TOK", sizeof(strbuf)); return DESTROY_TOK; }
"public"        { strncpy(strbuf, "PUBLIC_TOK", sizeof(strbuf)); return PUBLIC_TOK; }
"private"       { strncpy(strbuf, "PRIVATE_TOK", sizeof(strbuf)); return PRIVATE_TOK; }
"protected"     { strncpy(strbuf, "PROTECTED_TOK", sizeof(strbuf)); return PROTECTED_TOK; }
"static"        { strncpy(strbuf, "STATIC_TOK", sizeof(strbuf)); return STATIC_TOK; }
"const"         { strncpy(strbuf, "CONST_TOK", sizeof(strbuf)); return CONST_TOK; }

    /* type cast operators */
"dict"          { strncpy(strbuf, "DICT_TOK", sizeof(strbuf)); return DICT_TOK; }
"array"         { strncpy(strbuf, "ARRAY_TOK", sizeof(strbuf)); return ARRAY_TOK; }
"bool"          { strncpy(strbuf, "BOOL_TOK", sizeof(strbuf)); return BOOL_TOK; }
"string"        { strncpy(strbuf, "STRING_TOK", sizeof(strbuf)); return STRING_TOK; }
"int8"          { strncpy(strbuf, "INT8_TOK", sizeof(strbuf)); return INT8_TOK; }
"int16"         { strncpy(strbuf, "INT16_TOK", sizeof(strbuf)); return INT16_TOK; }
"int32"         { strncpy(strbuf, "INT32_TOK", sizeof(strbuf)); return INT32_TOK; }
"int64"         { strncpy(strbuf, "INT64_TOK", sizeof(strbuf)); return INT64_TOK; }
"int"           { strncpy(strbuf, "INT_TOK", sizeof(strbuf)); return INT_TOK; }
"float"         { strncpy(strbuf, "FLOAT_TOK", sizeof(strbuf)); return FLOAT_TOK; }
"uint8"         { strncpy(strbuf, "UINT8_TOK", sizeof(strbuf)); return UINT8_TOK; }
"uint16"        { strncpy(strbuf, "UINT16_TOK", sizeof(strbuf)); return UINT16_TOK; }
"uint32"        { strncpy(strbuf, "UINT32_TOK", sizeof(strbuf)); return UINT32_TOK; }
"uint64"        { strncpy(strbuf, "UINT64_TOK", sizeof(strbuf)); return UINT64_TOK; }
"uint"          { strncpy(strbuf, "UINT_TOK", sizeof(strbuf)); return UINT_TOK; }

    /* flow operators */
"if"            { strncpy(strbuf, "IF_TOK", sizeof(strbuf)); return IF_TOK; }
"elseif"        { strncpy(strbuf, "ELSEIF_TOK", sizeof(strbuf)); return ELSEIF_TOK; }
"else"          { strncpy(strbuf, "ELSE_TOK", sizeof(strbuf)); return ELSE_TOK; }
"for"           { strncpy(strbuf, "FOR_TOK", sizeof(strbuf)); return FOR_TOK; }
"while"         { strncpy(strbuf, "WHILE_TOK", sizeof(strbuf)); return WHILE_TOK; }
"do"            { strncpy(strbuf, "DO_TOK", sizeof(strbuf)); return DO_TOK; }
"raise"         { strncpy(strbuf, "RAISE_TOK", sizeof(strbuf)); return RAISE_TOK; }
"except"        { strncpy(strbuf, "EXCEPT_TOK", sizeof(strbuf)); return EXCEPT_TOK; }
"switch"        { strncpy(strbuf, "SWITCH_TOK", sizeof(strbuf)); return SWITCH_TOK; }
"case"          { strncpy(strbuf, "CASE_TOK", sizeof(strbuf)); return CASE_TOK; }
"break"         { strncpy(strbuf, "BREAK_TOK", sizeof(strbuf)); return BREAK_TOK; }
"continue"      { strncpy(strbuf, "CONTINUE_TOK", sizeof(strbuf)); return CONTINUE_TOK; }

    /* boolean operator tokens */
"&&"            { strncpy(strbuf, "AND_TOK", sizeof(strbuf)); return AND_TOK; }
"||"            { strncpy(strbuf, "OR_TOK", sizeof(strbuf)); return OR_TOK; }
"!"             { strncpy(strbuf, "NOT_TOK", sizeof(strbuf)); return NOT_TOK; }
"=="            { strncpy(strbuf, "EQU_TOK", sizeof(strbuf)); return EQU_TOK; }
"!="            { strncpy(strbuf, "NEQU_TOK", sizeof(strbuf)); return NEQU_TOK; }
"<="            { strncpy(strbuf, "LTEQU_TOK", sizeof(strbuf)); return LTEQU_TOK; }
">="            { strncpy(strbuf, "GTEQU_TOK", sizeof(strbuf)); return GTEQU_TOK; }
"<"             { strncpy(strbuf, "LESS_TOK", sizeof(strbuf)); return LESS_TOK; }
">"             { strncpy(strbuf, "MORE_TOK", sizeof(strbuf)); return MORE_TOK; }

    /* arithmetic operators */
"-"             { strncpy(strbuf, "-", sizeof(strbuf)); return '-'; }
"+"             { strncpy(strbuf, "+", sizeof(strbuf)); return '+'; }
"="             { strncpy(strbuf, "=", sizeof(strbuf)); return '='; }
"/"             { strncpy(strbuf, "/", sizeof(strbuf)); return '/'; }
"*"             { strncpy(strbuf, "*", sizeof(strbuf)); return '*'; }
"%"             { strncpy(strbuf, "%%", sizeof(strbuf)); return '%'; }
"&"             { strncpy(strbuf, "&", sizeof(strbuf)); return '&'; }
"|"             { strncpy(strbuf, "|", sizeof(strbuf)); return '|'; }
"@"             { strncpy(strbuf, "@", sizeof(strbuf)); return '@'; }
"$"             { strncpy(strbuf, "$", sizeof(strbuf)); return '$'; }
"^"             { strncpy(strbuf, "^", sizeof(strbuf)); return '^'; }
"~"             { strncpy(strbuf, "~", sizeof(strbuf)); return '~'; }
"?"             { strncpy(strbuf, "?", sizeof(strbuf)); return '?'; }
"\."            { strncpy(strbuf, ".", sizeof(strbuf)); return '.'; }

    /* Other operators */
","             { strncpy(strbuf, ",", sizeof(strbuf)); return ','; }
";"             { strncpy(strbuf, ";", sizeof(strbuf)); return ';'; }
":"             { strncpy(strbuf, ":", sizeof(strbuf)); return ':'; }
"["             { strncpy(strbuf, "[", sizeof(strbuf)); return '['; }
"]"             { strncpy(strbuf, "]", sizeof(strbuf)); return ']'; }
"{"             { strncpy(strbuf, "{", sizeof(strbuf)); return '{'; }
"}"             { strncpy(strbuf, "}", sizeof(strbuf)); return '}'; }
"("             { strncpy(strbuf, "(", sizeof(strbuf)); return '('; }
")"             { strncpy(strbuf, ")", sizeof(strbuf)); return ')'; }

    /* constructed tokens */
[_A-Za-z][_0-9A-Za-z]*    { strncpy(strbuf, yytext, sizeof(strbuf)); return SYMBOL_TOK; }
[_A-Za-z][_0-9A-Za-z\.]*    { strncpy(strbuf, yytext, sizeof(strbuf)); return COMPLEX_TOK; }

    /* recognize and ignore a C comments */

#.*\n {name_stack->line_no ++; yylineno++;}

    /* recognize an integer */
[0-9]+    {
        strncpy(strbuf, yytext, sizeof(strbuf));
        return SNUM_TOK;
    }

    /* recognize an unsigned number */
0[xX][0-9A-Fa-f]+ {
        strncpy(strbuf, yytext, sizeof(strbuf));
        return UNUM_TOK;
    }

    /* recognize a float */
[0-9]+\.[0-9]+([eE]*[-+]?[0-9]+)? {
        strncpy(strbuf, yytext, sizeof(strbuf));
        return FNUM_TOK;
    }

    /* double quoted strings have escapes managed */
\"  {
        bidx = 0;
        memset(buffer, 0, sizeof(buffer));
        BEGIN(DQUOTES);
    }

<DQUOTES>\" {
        strncpy(strbuf, buffer, sizeof(strbuf));
        BEGIN(INITIAL);
        return QSTRG_TOK;
    }

    /* problem is that the short rule matches before the long one does */
    /* <DQUOTES>\"[ \t]*\\[ \t]*\n$ { printf("here2\n"); name_stack->line_no ++; } */
<DQUOTES>\\n    { append_char('\n'); }
<DQUOTES>\\r    { append_char('\r'); }
<DQUOTES>\\t    { append_char('\t'); }
<DQUOTES>\\b    { append_char('\b'); }
<DQUOTES>\\f    { append_char('\f'); }
<DQUOTES>\\v    { append_char('\v'); }
<DQUOTES>\\\\   { append_char('\\'); }
<DQUOTES>\\\"   { append_char('\"'); }
<DQUOTES>\\\'   { append_char('\''); }
<DQUOTES>\\\?   { append_char('\?'); }
<DQUOTES>\\.    { append_char(yytext[1]); }
<DQUOTES>\\[0-7]{1,3} { append_char((char)strtol(yytext+1, 0, 8));  }
<DQUOTES>\\[xX][0-9a-fA-F]{1,3} { append_char((char)strtol(yytext+2, 0, 16));  }
<DQUOTES>[^\\\"\n]*  { append_str(yytext); }


    /* single quoted strings are absolute literals */
\'  {
        bidx = 0;
        memset(buffer, 0, sizeof(buffer));
        BEGIN(SQUOTES);
    }

    /*<SQUOTES>\'[ \t]*\\[ \t]*\n { name_stack->line_no ++; } */
<SQUOTES>[^\\'\n]*  { append_str(yytext); }
<SQUOTES>\\.    { append_str(yytext); }
<SQUOTES>\' {
        strncpy(strbuf, buffer, sizeof(strbuf));
        BEGIN(INITIAL);
        yylval.str = strdup(strbuf);
        return QSTRG_TOK;
    }

<<EOF>> {

        show_debug_msg(0, "closing file \"%s\"", name_stack->name);
        fprintf(stderr, "There were %d errors found.\n", num_errors);
        _file_name_stack *name = name_stack->next;
        free(name_stack->name);
        free(name_stack);
        name_stack = name;

        memset(strbuf, 0, sizeof(strbuf));
        yypop_buffer_state();
        if(!YY_CURRENT_BUFFER) {
            yyterminate();
        }

        //return EOF_TOK;
    }

%%

void open_file(char *fname) {

    _file_name_stack *name;

    show_debug_msg(0, "opening file: \"%s\"", fname);
    if(NULL == (name = calloc(1, sizeof(_file_name_stack))))
        show_fatal_error("cannot allocate memory for file stack");

    if(NULL == (name->name = strdup(fname)))
        show_fatal_error("cannot allocate memory for file stack name");

    name->next = name_stack;
    name->line_no = 1;
    name_stack = name;

    yyin = fopen(fname, "r");
    if(NULL == yyin)
        show_fatal_error("cannot open the input file: \"%s\": %s", fname, strerror(errno));

    yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
}

// Tracking and global interface
char *get_file_name(void) {
    if(NULL != name_stack)
        return name_stack->name;
    else
        return "no name";
}

int get_line_number(void) {
    if(NULL != name_stack)
        return name_stack->line_no;
    else
        return -1;
}

const char *get_tok_str(void) {
    return strbuf;
}

int get_token(void) {

    strbuf[0] = 0;
    int retv = yylex();
    if(0 == retv)
        memset(strbuf, 0, sizeof(strbuf));
    //printf("%d: ", retv);
    return retv;
}
