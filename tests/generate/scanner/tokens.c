/**
 *
 * @file tokens.c
 *
 * @brief Token queue implementation.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */


#include <stdio.h>
#include <strings.h>

#include "tokens.h"
#include "scanner.h"
#include "queue.h"
#include "fileio.h"
#include "alloc.h"

queue_t* token_queue = NULL;

void init_token_queue(const char* fname) {

    if(token_queue == NULL)
        token_queue = _ALLOC_TYPE(queue_t);

    open_file(fname);
    yylex();
}

void* post_token_queue(void) {

    return (void*)token_queue->crnt;
}

void reset_token_queue(void* post) {

    token_queue->crnt = (queue_element_t*)post;
}

void consume_token_queue(void) {

    queue_element_t* next;
    for(queue_element_t* elem = token_queue->head; elem != token_queue->crnt; elem = next) {
        next = elem->next;
        destroy_token(elem->data);
        _FREE(elem);
    }
}

void add_token_queue(token_t* tok) {

    add_queue(token_queue, (void*)tok);
}

token_t* create_token(const char* str, token_type_t type) {

    token_t* tok = _ALLOC_TYPE(token_t);
    tok->type = type;
    tok->raw = create_string_buf(str);
    tok->line_no = get_line_no();
    tok->col_no = get_col_no();
    tok->fname = create_string_buf(get_file_name());

    return tok;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        if(tok->raw != NULL)
            destroy_string_buf(tok->raw);
        if(tok->fname != NULL)
            destroy_string_buf(tok->fname);

        _FREE(tok);
    }
}

token_t* get_token(void) {

    if(token_queue != NULL) {
        return peek_queue(token_queue);
    }
    else
        return NULL;
}

token_t* advance_token(void) {

    token_t* tok = advance_queue(token_queue);
    if(tok == NULL) {
        tok = get_token();
        if(tok->type != TOK_END_OF_INPUT) {
            if(yylex() == 0)
                add_token_queue(create_token("end of input", TOK_END_OF_INPUT));

            token_queue->crnt = token_queue->tail;
            tok = get_token();
        }
    }

    return tok;
}

const char* token_type_to_str(token_type_t type) {

    return (type == TOK_IMPORT)? "IMPORT" :
        (type == TOK_STRING_LITERAL)? "STRING LITERAL" :
        (type == TOK_IDENTIFIER)? "IDENTIFIER" :
        (type == TOK_EQUAL)? "EQUAL" :
        (type == TOK_CONST)? "CONST" :
        (type == TOK_OSBRACE)? "OSBRACE" :
        (type == TOK_CSBRACE)? "CSBRACE" :
        (type == TOK_OCBRACE)? "OCBRACE" :
        (type == TOK_CCBRACE)? "CCBRACE" :
        (type == TOK_COMMA)? "COMMA" :
        (type == TOK_COLON)? "COLON" :
        (type == TOK_OPAREN)? "OPAREN" :
        (type == TOK_CPAREN)? "CPAREN" :
        (type == TOK_INT)? "INT" :
        (type == TOK_FLOAT)? "FLOAT" :
        (type == TOK_STRING)? "STRING" :
        (type == TOK_LIST)? "LIST" :
        (type == TOK_DICT)? "DICT" :
        (type == TOK_BOOL)? "BOOL" :
        (type == TOK_DOT)? "DOT" :
        (type == TOK_NOTHING)? "NOTHING" :
        (type == TOK_STRUCT)? "STRUCT" :
        (type == TOK_STAR)? "STAR" :
        (type == TOK_SLASH)? "SLASH" :
        (type == TOK_PERCENT)? "PERCENT" :
        (type == TOK_PLUS)? "PLUS" :
        (type == TOK_MINUS)? "MINUS" :
        (type == TOK_GT)? "GT" :
        (type == TOK_LT)? "LT" :
        (type == TOK_GTE)? "GTE" :
        (type == TOK_LTE)? "LTE" :
        (type == TOK_EQU)? "EQU" :
        (type == TOK_NEQU)? "NEQU" :
        (type == TOK_AND)? "AND" :
        (type == TOK_OR)? "OR" :
        (type == TOK_CARAT)? "CARAT" :
        (type == TOK_NOT)? "NOT" :
        (type == TOK_UNARY)? "UNARY" :
        (type == TOK_INT_LITERAL)? "INT LITERAL" :
        (type == TOK_FLOAT_LITERAL)? "FLOAT LITERAL" :
        (type == TOK_INLINE)? "INLINE" :
        (type == TOK_CONTINUE)? "CONTINUE" :
        (type == TOK_BREAK)? "BREAK" :
        (type == TOK_IF)? "IF" :
        (type == TOK_ELSE)? "ELSE" :
        (type == TOK_WHILE)? "WHILE" :
        (type == TOK_DO)? "DO" :
        (type == TOK_FOR)? "FOR" :
        (type == TOK_IN)? "IN" :
        (type == TOK_RETURN)? "RETURN" :
        (type == TOK_EXIT)? "EXIT" :

        (type == TOK_END_OF_INPUT)? "END OF INPUT" :
        (type == TOK_END_OF_FILE)? "END OF FILE" :
        (type == TOK_ERROR)? "ERROR" : "UNKNOWN";
}

