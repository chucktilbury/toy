/**
 *
 * @file parser.h
 *
 * @brief Parse grammar public interface.
 * This file was generated on Wed Jul 16 21:15:55 2025.
 *
 */
#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"
#include "parser_prototypes.h"

typedef enum {
    STATE_START = 0,
    STATE_MATCH = 1000,
    STATE_NO_MATCH = 1010,
    STATE_ERROR = 1020,
} parser_state_t;
typedef struct {
    int mode;
} parser_state_t;

#define STATE \ 
    do { \ 
        TRACE("STATE: %d", state); \ 
    } while(0)

ast_translation_unit_t* parse(void);

#endif /* _PARSER_H_ */

