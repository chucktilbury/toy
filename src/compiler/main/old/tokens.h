#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <stdbool.h>
#include "string_buffer.h"

typedef enum {
    TOK_CONST = 256,    // 'const'
    TOK_NOTHING,        // 'nothing'
    TOK_IMPORT,         // 'import'
    TOK_INT,            // 'int'
    TOK_FLOAT,          // 'float'
    TOK_STRING,         // 'string'
    TOK_BOOL,           // 'bool'
    TOK_LIST,           // 'list'
    TOK_DICT,           // 'dict'
    TOK_STRUCT,         // 'struct'
    TOK_DO,             // 'do'
    TOK_WHILE,          // 'while'
    TOK_FOR,            // 'for'
    TOK_IN,             // 'in'
    TOK_IF,             // 'if'
    TOK_ELSE,           // 'else'
    TOK_RETURN,         // 'return'
    TOK_EXIT,           // 'exit'
    TOK_CONTINUE,       // 'continue'
    TOK_BREAK,          // 'break'
    TOK_IDENTIFIER,     // IDENTIFIER
    TOK_INT_LITERAL,    // INT_LITERAL
    TOK_FLOAT_LITERAL,  // FLOAT_LITERAL
    TOK_STRING_LITERAL, // STRING_LITERAL
    TOK_INLINE,         // INLINE
    TOK_COLON,          // ':'
    TOK_OR,             // 'or' | '|'
    TOK_AND,            // 'and' | '&'
    TOK_EQU,            // 'equ' | '=='
    TOK_NEQU,           // 'nequ' | '!='
    TOK_GT,             // 'gt' | '>'
    TOK_LT,             //  'lt' | '<'
    TOK_GTE,            // 'gte' | '>='
    TOK_LTE,            // 'lte' | '<='
    TOK_PLUS,           // '+'
    TOK_MINUS,          // '-'
    TOK_STAR,           // '*'
    TOK_SLASH,          // '/'
    TOK_PERCENT,        // '%'
    TOK_CARAT,          // '^'
    TOK_NOT,            // 'not' | '!'
    TOK_DOT,            // '.'
    TOK_OSBRACE,        // '['
    TOK_CSBRACE,        // ']'
    TOK_COMMA,          // ','
    TOK_OCBRACE,        // '{'
    TOK_CCBRACE,        // '}'
    TOK_EQUAL,          // '='
    TOK_OPAREN,         // '('
    TOK_CPAREN,         // ')'
    TOK_END_OF_INPUT,
    TOK_END_OF_FILE,
} token_type_t;

typedef struct _token_t_ {
    token_type_t type;
    string_t* str;
    const char* fname;
    int line_no;
    int col_no;
    struct _token_t_* next;
} token_t;

token_t* create_token(string_t* str, token_type_t type);
void destroy_token(token_t* tok);
const char* tok_type_to_str(token_t* tok);
const char* tok_name_to_str(token_t* tok);

void init_token_queue(void);
void destroy_token_queue(void);
void add_token_queue(token_t* tok);
void* mark_token_queue(void);
void restore_token_queue(void* mark);
void consume_token_queue(void);

token_t* get_token(void);
bool expect_token(token_type_t type);
token_t* consume_token(void);

#endif /* _TOKENS_H_ */
