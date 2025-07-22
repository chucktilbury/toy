
#include "tokens.h"
#include "scanner.h"
#include "alloc.h"
#include "file_io.h"

// there is one global token queue
typedef struct _token_queue_t_ {
    token_t* head;
    token_t* tail;
    token_t* crnt;
} token_queue_t;

static token_queue_t* token_queue = NULL;
static token_t end_of_input;

token_t* create_token(string_t* str, token_type_t type) {

    token_t* ptr = _ALLOC_TYPE(token_t);
    ptr->str = copy_string(str);
    ptr->type = type;
    ptr->fname = raw_string((string_t*)get_file_name());
    ptr->line_no = get_line_no();
    ptr->col_no = get_col_no();

    return ptr;
}

void destroy_token(token_t* tok) {

    if(tok != NULL) {
        destroy_string(tok->str);
        _FREE(tok);
        // note that the fname is allocated by the fileio
    }
}

void init_token_queue(void) {

    token_queue = _ALLOC_TYPE(token_queue_t);
    // add_token_queue(get_scanner_token());
    yylex();

    end_of_input.type = TOK_END_OF_INPUT;
    end_of_input.str = create_string(NULL);
    // everything else is NULL;
}

void destroy_token_queue(void) {

    if(token_queue != NULL) {
        token_t* crnt;
        token_t* next;
        for(crnt = token_queue->head; crnt != NULL; crnt = next) {
            next = crnt->next;
            destroy_token(crnt);
        }
        _FREE(token_queue);
    }
}

void add_token_queue(token_t* tok) {

    if(token_queue->tail != NULL)
        token_queue->tail->next = tok;
    else {
        token_queue->head = tok;
        token_queue->crnt = tok;
    }
    token_queue->tail = tok;
}

void* mark_token_queue(void) {

    if(token_queue != NULL)
        return token_queue->crnt;
    else
        return NULL;
}

void restore_token_queue(void* mark) {

    if(token_queue != NULL)
        token_queue->crnt = (token_t*)mark;
}

void consume_token_queue(void) {

    if(token_queue != NULL) {
        if(token_queue->crnt != NULL && token_queue->crnt != token_queue->head) {
            token_t* next;
            token_t* tok = token_queue->head;
            for(; tok != NULL && tok != token_queue->crnt; tok = next) {
                next = tok->next;
                destroy_token(tok);
            }

            token_queue->head = token_queue->crnt;
        }
    }
}

token_t* get_token(void) {

    if(token_queue != NULL && token_queue->crnt != NULL)
        return token_queue->crnt;
    else
        return &end_of_input;
}

token_t* consume_token(void) {

    if(token_queue != NULL)
        if(token_queue->crnt != NULL)
            token_queue->crnt = token_queue->crnt->next;

    if(token_queue->crnt == NULL) {
        // accommodate a FLEX scanner
        // add_token_queue(get_scanner_token());
        yylex();
        token_queue->crnt = token_queue->tail;
    }

    return get_token();
}

bool expect_token(token_type_t type) {

    token_t* tok = get_token();
    return (tok->type == type) ? true : false;
}

const char* tok_name_to_str(token_t* tok) {

    return (tok->type == TOK_CONST)           ? "const" :
            (tok->type == TOK_NOTHING)        ? "nothing" :
            (tok->type == TOK_IMPORT)         ? "import" :
            (tok->type == TOK_INT)            ? "int" :
            (tok->type == TOK_FLOAT)          ? "float" :
            (tok->type == TOK_STRING)         ? "string" :
            (tok->type == TOK_BOOL)           ? "bool" :
            (tok->type == TOK_LIST)           ? "list" :
            (tok->type == TOK_DICT)           ? "dict" :
            (tok->type == TOK_STRUCT)         ? "struct" :
            (tok->type == TOK_DO)             ? "do" :
            (tok->type == TOK_WHILE)          ? "while" :
            (tok->type == TOK_FOR)            ? "for" :
            (tok->type == TOK_IN)             ? "in" :
            (tok->type == TOK_IF)             ? "if" :
            (tok->type == TOK_ELSE)           ? "else" :
            (tok->type == TOK_RETURN)         ? "return" :
            (tok->type == TOK_EXIT)           ? "exit" :
            (tok->type == TOK_CONTINUE)       ? "continue" :
            (tok->type == TOK_BREAK)          ? "break" :
            (tok->type == TOK_COLON)          ? ":" :
            (tok->type == TOK_PLUS)           ? "+" :
            (tok->type == TOK_MINUS)          ? "-" :
            (tok->type == TOK_STAR)           ? "*" :
            (tok->type == TOK_SLASH)          ? "/" :
            (tok->type == TOK_PERCENT)        ? "%" :
            (tok->type == TOK_CARAT)          ? "^" :
            (tok->type == TOK_DOT)            ? "." :
            (tok->type == TOK_OSBRACE)        ? "[" :
            (tok->type == TOK_CSBRACE)        ? "]" :
            (tok->type == TOK_COMMA)          ? "," :
            (tok->type == TOK_OCBRACE)        ? "{" :
            (tok->type == TOK_CCBRACE)        ? "}" :
            (tok->type == TOK_EQUAL)          ? "=" :
            (tok->type == TOK_OPAREN)         ? "(" :
            (tok->type == TOK_CPAREN)         ? ")" :
            (tok->type == TOK_NOT)            ? "not" :
            (tok->type == TOK_OR)             ? "or" :
            (tok->type == TOK_AND)            ? "and" :
            (tok->type == TOK_EQU)            ? "equ" :
            (tok->type == TOK_NEQU)           ? "nequ" :
            (tok->type == TOK_GT)             ? "gt" :
            (tok->type == TOK_LT)             ? "lt" :
            (tok->type == TOK_GTE)            ? "gte" :
            (tok->type == TOK_LTE)            ? "lte" :
            (tok->type == TOK_IDENTIFIER)     ? "IDENTIFIER" :
            (tok->type == TOK_INT_LITERAL)    ? "INT_LITERAL" :
            (tok->type == TOK_FLOAT_LITERAL)  ? "FLOAT_LITERAL" :
            (tok->type == TOK_STRING_LITERAL) ? "STRING_LITERAL" :
            (tok->type == TOK_INLINE)         ? "INLINE" :
            (tok->type == TOK_END_OF_INPUT)   ? "END_OF_INPUT" :
            (tok->type == TOK_END_OF_FILE)    ? "END_OF_FILE" :
                                                "UNKNOWN";
}

const char* tok_type_to_str(token_t* tok) {

    return (tok->type == TOK_CONST)           ? "CONST" :
            (tok->type == TOK_NOTHING)        ? "NOTHING" :
            (tok->type == TOK_IMPORT)         ? "IMPORT" :
            (tok->type == TOK_INT)            ? "INT" :
            (tok->type == TOK_FLOAT)          ? "FLOAT" :
            (tok->type == TOK_STRING)         ? "STRING" :
            (tok->type == TOK_BOOL)           ? "BOOL" :
            (tok->type == TOK_LIST)           ? "LIST" :
            (tok->type == TOK_DICT)           ? "DICT" :
            (tok->type == TOK_STRUCT)         ? "STRUCT" :
            (tok->type == TOK_DO)             ? "DO" :
            (tok->type == TOK_WHILE)          ? "WHILE" :
            (tok->type == TOK_FOR)            ? "FOR" :
            (tok->type == TOK_IN)             ? "IN" :
            (tok->type == TOK_IF)             ? "IF" :
            (tok->type == TOK_ELSE)           ? "ELSE" :
            (tok->type == TOK_RETURN)         ? "RETURN" :
            (tok->type == TOK_EXIT)           ? "EXIT" :
            (tok->type == TOK_CONTINUE)       ? "CONTINUE" :
            (tok->type == TOK_BREAK)          ? "BREAK" :
            (tok->type == TOK_COLON)          ? "COLON" :
            (tok->type == TOK_PLUS)           ? "PLUS" :
            (tok->type == TOK_MINUS)          ? "MINUS" :
            (tok->type == TOK_STAR)           ? "STAR" :
            (tok->type == TOK_SLASH)          ? "SLASH" :
            (tok->type == TOK_PERCENT)        ? "PERCENT" :
            (tok->type == TOK_CARAT)          ? "CARAT" :
            (tok->type == TOK_DOT)            ? "DOT" :
            (tok->type == TOK_OSBRACE)        ? "OSBRACE" :
            (tok->type == TOK_CSBRACE)        ? "CSBRACE" :
            (tok->type == TOK_COMMA)          ? "COMMA" :
            (tok->type == TOK_OCBRACE)        ? "OCBRACE" :
            (tok->type == TOK_CCBRACE)        ? "CCBRACE" :
            (tok->type == TOK_EQUAL)          ? "EQUAL" :
            (tok->type == TOK_OPAREN)         ? "OPAREN" :
            (tok->type == TOK_CPAREN)         ? "CPAREN" :
            (tok->type == TOK_NOT)            ? "NOT" :
            (tok->type == TOK_OR)             ? "OR" :
            (tok->type == TOK_AND)            ? "AND" :
            (tok->type == TOK_EQU)            ? "EQU" :
            (tok->type == TOK_NEQU)           ? "NEQU" :
            (tok->type == TOK_GT)             ? "GT" :
            (tok->type == TOK_LT)             ? "LT" :
            (tok->type == TOK_GTE)            ? "GTE" :
            (tok->type == TOK_LTE)            ? "LTE" :
            (tok->type == TOK_IDENTIFIER)     ? "IDENTIFIER" :
            (tok->type == TOK_INT_LITERAL)    ? "INT_LITERAL" :
            (tok->type == TOK_FLOAT_LITERAL)  ? "FLOAT_LITERAL" :
            (tok->type == TOK_STRING_LITERAL) ? "STRING_LITERAL" :
            (tok->type == TOK_INLINE)         ? "INLINE" :
            (tok->type == TOK_END_OF_INPUT)   ? "END_OF_INPUT" :
            (tok->type == TOK_END_OF_FILE)    ? "END_OF_FILE" :
                                                "UNKNOWN";
}
