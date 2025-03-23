#ifndef _TYPE_TABLE_
#define _TYPE_TABLE_

#include "stdbool.h"
#include "tokens.h"
#include "ast.h"

int check_assignment_type(token_t*, int, int);
int check_binary_expression_type(token_t*, int, int);
int check_unary_expression_type(token_t*, int);
int check_expression_cast(ast_type_name_t*, ast_expression_t*);

#endif /* _TYPE_TABLE_ */
