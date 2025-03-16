

#ifndef _SYNTAX_ERRORS_H_
#define _SYNTAX_ERRORS_H_

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "ast.h"

void fatal_error(const char* fmt, ...);
void syntax_error(token_t* tok, const char* fmt, ...);
void syntax_warning(token_t* tok, const char* fmt, ...);
void node_syntax(ast_node_t*, const char*, ...);
void misc_error(const char* fmt, ...);
int get_errors(void);
int get_warnings(void);
void increment_errors(void);

#endif /* _SYNTAX_ERRORS_H_ */
