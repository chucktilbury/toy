#ifndef _TOKENS_H_
#define _TOKENS_H_

#include <stdbool.h>
#include "string_buffer.h"

typedef enum {
    CONST=256,  // 'const'
    NOTHING,    // 'nothing'
    IMPORT,     // 'import'
    INT,        // 'int'
    FLOAT,      // 'float'
    STRING,     // 'string'
    BOOL,       // 'bool'
    LIST,       // 'list'
    DICT,       // 'dict'
    STRUCT,     // 'struct'
    DO,         // 'do'
    WHILE,      // 'while'
    FOR,        // 'for'
    IN,         // 'in'
    IF,         // 'if'
    ELSE,       // 'else'
    RETURN,     // 'return'
    EXIT,       // 'exit'
    CONTINUE,   // 'continue'
    BREAK,      // 'break'
    IDENTIFIER,     // IDENTIFIER
    INT_LITERAL,    // INT_LITERAL
    FLOAT_LITERAL,  // FLOAT_LITERAL
    STRING_LITERAL, // STRING_LITERAL
    INLINE,     // INLINE
    COLON,      // ':'
    OR,         // 'or' | '|'
    AND,        // 'and' | '&'
    EQU,        // 'equ' | '=='
    NEQU,       // 'nequ' | '!='
    GT,         // 'gt' | '>'
    LT,         //  'lt' | '<'
    GTE,        // 'gte' | '>='
    LTE,        // 'lte' | '<='
    PLUS,       // '+'
    MINUS,      // '-'
    STAR,       // '*'
    SLASH,      // '/'
    PERCENT,    // '%'
    CARAT,      // '^'
    NOT,        // 'not' | '!'
    DOT,        // '.'
    OSBRACE,    // '['
    CSBRACE,    // ']'
    COMMA,      // ','
    OCBRACE,    // '{'
    CCBRACE,    // '}'
    EQUAL,      // '='
    OPAREN,     // '('
    CPAREN,     // ')'
    END_OF_INPUT,
    END_OF_FILE,
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
