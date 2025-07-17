/**
 *
 * @file token_defs.h
 *
 * @brief Token definitions public interface.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#ifndef _TOKEN_DEFS_H_
#define _TOKEN_DEFS_H_

#include <stdint.h>
#include "string_buf.h"

typedef enum {
    TOK_IMPORT = 256,
    TOK_STRING_LITERAL = 257,
    TOK_IDENTIFIER = 258,
    TOK_EQUAL = 259,
    TOK_CONST = 260,
    TOK_OSBRACE = 261,
    TOK_CSBRACE = 262,
    TOK_OCBRACE = 263,
    TOK_CCBRACE = 264,
    TOK_COMMA = 265,
    TOK_COLON = 266,
    TOK_OPAREN = 267,
    TOK_CPAREN = 268,
    TOK_INT = 269,
    TOK_FLOAT = 270,
    TOK_STRING = 271,
    TOK_LIST = 272,
    TOK_DICT = 273,
    TOK_BOOL = 274,
    TOK_DOT = 275,
    TOK_NOTHING = 276,
    TOK_STRUCT = 277,
    TOK_STAR = 278,
    TOK_SLASH = 279,
    TOK_PERCENT = 280,
    TOK_PLUS = 281,
    TOK_MINUS = 282,
    TOK_GT = 283,
    TOK_LT = 284,
    TOK_GTE = 285,
    TOK_LTE = 286,
    TOK_EQU = 287,
    TOK_NEQU = 288,
    TOK_AND = 289,
    TOK_OR = 290,
    TOK_CARAT = 291,
    TOK_NOT = 292,
    TOK_UNARY = 293,
    TOK_INT_LITERAL = 294,
    TOK_FLOAT_LITERAL = 295,
    TOK_INLINE = 296,
    TOK_CONTINUE = 297,
    TOK_BREAK = 298,
    TOK_IF = 299,
    TOK_ELSE = 300,
    TOK_WHILE = 301,
    TOK_DO = 302,
    TOK_FOR = 303,
    TOK_IN = 304,
    TOK_RETURN = 305,
    TOK_EXIT = 306,

    TOK_END_OF_INPUT = 307,
    TOK_END_OF_FILE = 308,
    TOK_ERROR = 309,
} token_type_t;


typedef struct _token_t_ {
    token_type_t type;
    string_buf_t* raw;
    string_buf_t* fname;
    int line_no;
    int col_no;
} token_t;

void init_token_queue(const char* fname);
void* post_token_queue(void);
void reset_token_queue(void* post);
void consume_token_queue(void);
void add_token_queue(token_t* tok);

token_t* create_token(const char* str, token_type_t type);
void destroy_token(token_t* tok);
token_t* get_token(void);
token_t* advance_token(void);
const char* token_type_to_str(token_type_t type);

#endif /* _TOKEN_DEFS_H_ */

