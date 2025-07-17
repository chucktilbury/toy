%{

#include "tokens.h"
#include "parser.h"
#include "string_buffer.h"
#include "file_io.h"

int depth;
string_t* strbuf = NULL;
int yycolno = 1;
int prev_lineno;

#define YY_USER_ACTION update_numbers();

%}

%option yylineno
%option noinput
%option nounput
%option noyywrap
    // for cmake...
    //%option header-file="scanner.h"
    //%option outfile="scanner.c"

%x CODEBLOCK

%%
    // happens every time yylex() is called
    prev_lineno = yylineno;

"const"     { add_token_queue(create_token(create_string(yytext), CONST)); return CONST; }
"nothing"   { add_token_queue(create_token(create_string(yytext), NOTHING)); return NOTHING; }
"import"    { add_token_queue(create_token(create_string(yytext), IMPORT)); return IMPORT; }
"int"       { add_token_queue(create_token(create_string(yytext), INT)); return INT; }
"float"     { add_token_queue(create_token(create_string(yytext), FLOAT)); return FLOAT; }
"string"    { add_token_queue(create_token(create_string(yytext), STRING)); return STRING; }
"bool"      { add_token_queue(create_token(create_string(yytext), BOOL)); return BOOL; }
"list"      { add_token_queue(create_token(create_string(yytext), LIST)); return LIST; }
"dict"      { add_token_queue(create_token(create_string(yytext), DICT)); return DICT; }
"struct"    { add_token_queue(create_token(create_string(yytext), STRUCT)); return STRUCT; }
"do"        { add_token_queue(create_token(create_string(yytext), DO)); return DO; }
"while"     { add_token_queue(create_token(create_string(yytext), WHILE)); return WHILE; }
"for"       { add_token_queue(create_token(create_string(yytext), FOR)); return FOR; }
"in"        { add_token_queue(create_token(create_string(yytext), IN)); return IN; }
"if"        { add_token_queue(create_token(create_string(yytext), IF)); return IF; }
"else"      { add_token_queue(create_token(create_string(yytext), ELSE)); return ELSE; }
"return"    { add_token_queue(create_token(create_string(yytext), RETURN)); return RETURN; }
"exit"      { add_token_queue(create_token(create_string(yytext), EXIT)); return EXIT; }

"or"|"|"    { add_token_queue(create_token(create_string(yytext), OR)); return OR; }
"and"|"&"   { add_token_queue(create_token(create_string(yytext), AND)); return AND; }
"equ"|"=="  { add_token_queue(create_token(create_string(yytext), EQU)); return EQU; }
"nequ"|"!=" { add_token_queue(create_token(create_string(yytext), NEQU)); return NEQU; }
"gt"|">"    { add_token_queue(create_token(create_string(yytext), GT)); return GT; }
"lt"|"<"    { add_token_queue(create_token(create_string(yytext), LT)); return LT; }
"gte"|">="  { add_token_queue(create_token(create_string(yytext), GTE)); return GTE; }
"lte"|"<="  { add_token_queue(create_token(create_string(yytext), LTE)); return LTE; }
"not"|"!"   { add_token_queue(create_token(create_string(yytext), NOT)); return NOT; }
"-" { add_token_queue(create_token(create_string(yytext), MINUS)); return MINUS; }
"/" { add_token_queue(create_token(create_string(yytext), SLASH)); return SLASH; }
"%" { add_token_queue(create_token(create_string(yytext), PERCENT)); return PERCENT; }
"^" { add_token_queue(create_token(create_string(yytext), CARAT)); return CARAT; }
"[" { add_token_queue(create_token(create_string(yytext), OSBRACE)); return OSBRACE; }
"]" { add_token_queue(create_token(create_string(yytext), CSBRACE)); return CSBRACE; }
"," { add_token_queue(create_token(create_string(yytext), COMMA)); return COMMA; }
"{" { add_token_queue(create_token(create_string(yytext), OCBRACE)); return OCBRACE; }
"}" { add_token_queue(create_token(create_string(yytext), CCBRACE)); return CCBRACE; }
"=" { add_token_queue(create_token(create_string(yytext), EQUAL)); return EQUAL; }
"+" { add_token_queue(create_token(create_string(yytext), PLUS)); return PLUS; }
"*" { add_token_queue(create_token(create_string(yytext), STAR)); return STAR; }
"(" { add_token_queue(create_token(create_string(yytext), OPAREN)); return OPAREN; }
")" { add_token_queue(create_token(create_string(yytext), CPAREN)); return CPAREN; }
":" { add_token_queue(create_token(create_string(yytext), COLON)); return COLON; }

\. { add_token_queue(create_token(create_string(yytext), DOT)); return DOT; }

"inline"[ \t\r\n\v\f]*"{" {
    depth = 0;
    if(strbuf == NULL)
        strbuf = create_string(NULL);
    else
        clear_string(strbuf);
    BEGIN(CODEBLOCK);
}

<CODEBLOCK>"{" {
    depth++;
    append_string_char(strbuf, '{');
}

<CODEBLOCK>[^{}\n] {
    append_string(strbuf, yytext);
}

<CODEBLOCK>\n {
    // so line numbers continue to work
    append_string_char(strbuf, '\n');
}

<CODEBLOCK>"}" {
    if(depth > 0) {
        depth--;
        append_string_char(strbuf, '}');
    }
    else {
        BEGIN(INITIAL);
        add_token_queue(create_token(strbuf, INLINE));
        return INLINE;
    }
}

[a-zA-Z_][a-zA-Z0-9_]* {
    add_token_queue(create_token(create_string(yytext), IDENTIFIER));
    return IDENTIFIER;
}

([1-9][0-9]*)|0 {
    add_token_queue(create_token(create_string(yytext), INT_LITERAL));
    return INT_LITERAL;
}

(([1-9][0-9]*)|0)?\.[0-9]*([eE][-+]?[0-9]+)? {
    add_token_queue(create_token(create_string(yytext), FLOAT_LITERAL));
    return FLOAT_LITERAL;
}

\"[^\"\n]*\" {
    add_token_queue(create_token(strip_ends(create_string(yytext), '\"'), STRING_LITERAL));
    return STRING_LITERAL;
}

\'[^\'\n]*\' {
    add_token_queue(create_token(strip_ends(create_string(yytext), '\''), STRING_LITERAL));
    return STRING_LITERAL;
}

("#"|";")[^\n]* { /* ignore comment */ }

[ \t\r\n\v\f]* { /* ignore whitespace */ }

. {
    fprintf(stderr, "scanner error: unknown character encountered: '%c' (0x%02X)\n", yytext[0], yytext[0]);
}

<<EOF>> {
    add_token_queue(create_token(create_string(NULL), END_OF_FILE));
    yyterminate();
}

%%