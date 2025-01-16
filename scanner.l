
%{
#include "parser.h"
#include "memory.h"
%}

%option yylineno
%option noinput
%option noyywrap
%option header-file="scanner.h"
%option outfile="scanner.c"

%%

"integer"   { return INTEGER; }
"int"       { return INTEGER; }
"float"     { return FLOAT; }
"string"    { return STRING; }
"list"      { return LIST; }
"hash"      { return HASH; }
"filer"     { return FILER; }
"filew"     { return FILEW; }
"filerw"    { return FILERW; }
"nothing"   { return NOTHING; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"try"       { return TRY; }
"except"    { return EXCEPT; }
"raise"     { return RAISE; }
"final"     { return FINAL; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
"return"    { return RETURN; }
"print"     { return PRINT; }
"trace"     { return TRACE; }
"exit"      { return EXIT; }

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
"="     { return ASSIGN_OPER; }
"+"     { return ADD_OPER; }
"-"     { return SUB_OPER; }
"*"     { return MUL_OPER; }
"/"     { return DIV_OPER; }
"%"     { return MOD_OPER; }
"^"     { return POW_OPER; }
"("     { return OPAREN; }
")"     { return CPAREN; }
"{"     { return OCBRACE; }
"}"     { return CCBRACE; }
"["     { return OSBRACE; }
"]"     { return CSBRACE; }
","     { return COMMA; }
"."     { return DOT; }
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
    yylval.text = _COPY_STRING(yytext);
    return STRING_LIT;
}

\'[^\'\n]*\' {
    yylval.text = _COPY_STRING(yytext);
    return STRING_LIT;
}

#[^\n]*\n  { /* comment */ }

[ \t\n\r]+ { /* ignore spaces */ }

. {
        fprintf(stderr, "Error at %d: unrecognized character \"%s\"\n", yylineno, yytext);
        exit(0);
    }

%%

