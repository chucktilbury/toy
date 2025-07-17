
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
    //add_token_queue(get_scanner_token());
    yylex();

    end_of_input.type = END_OF_INPUT;
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
    return (tok->type == type)? true: false;
}

const char* tok_name_to_str(token_t* tok) {

    return (tok->type == CONST)? "const" :
        (tok->type == NOTHING)? "nothing" :
        (tok->type == IMPORT)? "import" :
        (tok->type == INT)? "int" :
        (tok->type == FLOAT)? "float" :
        (tok->type == STRING)? "string" :
        (tok->type == BOOL)? "bool" :
        (tok->type == LIST)? "list" :
        (tok->type == DICT)? "dict" :
        (tok->type == STRUCT)? "struct" :
        (tok->type == DO)? "do" :
        (tok->type == WHILE)? "while" :
        (tok->type == FOR)? "for" :
        (tok->type == IN)? "in" :
        (tok->type == IF)? "if" :
        (tok->type == ELSE)? "else" :
        (tok->type == RETURN)? "return" :
        (tok->type == EXIT)? "exit" :
        (tok->type == CONTINUE)? "continue" :
        (tok->type == BREAK)? "break" :
        (tok->type == COLON)?  ":" :
        (tok->type == PLUS)?  "+" :
        (tok->type == MINUS)?  "-" :
        (tok->type == STAR)?  "*" :
        (tok->type == SLASH)?  "/" :
        (tok->type == PERCENT)?  "%" :
        (tok->type == CARAT)?  "^" :
        (tok->type == DOT)?  "." :
        (tok->type == OSBRACE)?  "[" :
        (tok->type == CSBRACE)?  "]" :
        (tok->type == COMMA)?  "," :
        (tok->type == OCBRACE)?  "{" :
        (tok->type == CCBRACE)?  "}" :
        (tok->type == EQUAL)?  "=" :
        (tok->type == OPAREN)?  "(" :
        (tok->type == CPAREN)?  ")" :
        (tok->type == NOT)? "not" :
        (tok->type == OR)? "or" :
        (tok->type == AND)? "and" :
        (tok->type == EQU)? "equ" :
        (tok->type == NEQU)? "nequ" :
        (tok->type == GT)? "gt" :
        (tok->type == LT)? "lt" :
        (tok->type == GTE)? "gte" :
        (tok->type == LTE)? "lte" :
        (tok->type == IDENTIFIER)? "IDENTIFIER" :
        (tok->type == INT_LITERAL)? "INT_LITERAL" :
        (tok->type == FLOAT_LITERAL)? "FLOAT_LITERAL" :
        (tok->type == STRING_LITERAL)? "STRING_LITERAL" :
        (tok->type == INLINE)? "INLINE" :
        (tok->type == END_OF_INPUT)? "END_OF_INPUT" :
        (tok->type == END_OF_FILE)? "END_OF_FILE" :
        "UNKNOWN";
}

const char* tok_type_to_str(token_t* tok) {

    return
        (tok->type == CONST)? "CONST" :
        (tok->type == NOTHING)? "NOTHING" :
        (tok->type == IMPORT)? "IMPORT" :
        (tok->type == INT)? "INT" :
        (tok->type == FLOAT)? "FLOAT" :
        (tok->type == STRING)? "STRING" :
        (tok->type == BOOL)? "BOOL" :
        (tok->type == LIST)? "LIST" :
        (tok->type == DICT)? "DICT" :
        (tok->type == STRUCT)? "STRUCT" :
        (tok->type == DO)? "DO" :
        (tok->type == WHILE)? "WHILE" :
        (tok->type == FOR)? "FOR" :
        (tok->type == IN)? "IN" :
        (tok->type == IF)? "IF" :
        (tok->type == ELSE)? "ELSE" :
        (tok->type == RETURN)? "RETURN" :
        (tok->type == EXIT)? "EXIT" :
        (tok->type == CONTINUE)? "CONTINUE" :
        (tok->type == BREAK)? "BREAK" :
        (tok->type == COLON)?  "COLON" :
        (tok->type == PLUS)?  "PLUS" :
        (tok->type == MINUS)?  "MINUS" :
        (tok->type == STAR)?  "STAR" :
        (tok->type == SLASH)?  "SLASH" :
        (tok->type == PERCENT)?  "PERCENT" :
        (tok->type == CARAT)?  "CARAT" :
        (tok->type == DOT)?  "DOT" :
        (tok->type == OSBRACE)?  "OSBRACE" :
        (tok->type == CSBRACE)?  "CSBRACE" :
        (tok->type == COMMA)?  "COMMA" :
        (tok->type == OCBRACE)?  "OCBRACE" :
        (tok->type == CCBRACE)?  "CCBRACE" :
        (tok->type == EQUAL)?  "EQUAL" :
        (tok->type == OPAREN)?  "OPAREN" :
        (tok->type == CPAREN)?  "CPAREN" :
        (tok->type == NOT)? "NOT" :
        (tok->type == OR)? "OR" :
        (tok->type == AND)? "AND" :
        (tok->type == EQU)? "EQU" :
        (tok->type == NEQU)? "NEQU" :
        (tok->type == GT)? "GT" :
        (tok->type == LT)? "LT" :
        (tok->type == GTE)? "GTE" :
        (tok->type == LTE)? "LTE" :
        (tok->type == IDENTIFIER)? "IDENTIFIER" :
        (tok->type == INT_LITERAL)? "INT_LITERAL" :
        (tok->type == FLOAT_LITERAL)? "FLOAT_LITERAL" :
        (tok->type == STRING_LITERAL)? "STRING_LITERAL" :
        (tok->type == INLINE)? "INLINE" :
        (tok->type == END_OF_INPUT)? "END_OF_INPUT" :
        (tok->type == END_OF_FILE)? "END_OF_FILE" :
        "UNKNOWN";
}
