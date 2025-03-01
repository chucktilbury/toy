
#include "type_table.h"
#include "errors.h"
#include "parser.h"

// #define USE_TRACE
#include "trace.h"

/*
 * Compare the left and right for assignment, and return the effective type.
 * Issue a warning if the effective type does not match the left.
 */
int check_assignment_type(token_t* tok, int lhs, int rhs) {

    int val = -1;

    if(lhs == 0 || rhs == 0)
        return val;

    TRACE("checking %s <- %s", token_to_str(lhs), token_to_str(rhs));
    switch(lhs) {
        case INTEGER:

            switch(rhs) {
                case INTEGER:
                    val = INTEGER;
                    break;

                case FLOAT:
                    syntax_warning(tok, "assigning a float to an integer loses resolution");
                    val = INTEGER;
                    break;

                case STRING:
                    syntax_error(tok, "cannot assign a string to an integer");
                    break;

                case BOOL:
                    syntax_error(tok, "cannot assign an boolean to an integer");
                    break;

                case NOTHING:
                    syntax_error(tok, "cannot assign nothing to an integer");
                    break;

                default:
                    FATAL("unknown right hand type: %s (%d)", token_to_str(rhs), rhs);
            }
            break;
        case FLOAT:

            switch(rhs) {
                case INTEGER:
                    // type promotion does not issue a warning
                    val = FLOAT;
                    break;

                case FLOAT:
                    val = FLOAT;
                    break;

                case STRING:
                    syntax_error(tok, "cannot assign a string to an float");
                    break;

                case BOOL:
                    syntax_error(tok, "cannot assign an boolean to an float");
                    break;

                case NOTHING:
                    syntax_error(tok, "cannot assign nothing to a float");
                    break;

                default:
                    FATAL("unknown right hand type: %s (%d)", token_to_str(rhs), rhs);
            }
            break;

        case STRING:
            switch(rhs) {
                case INTEGER:
                    syntax_error(tok, "cannot assign an integer to an string");
                    break;

                case FLOAT:
                    syntax_error(tok, "cannot assign an float to an string");
                    break;

                case STRING:
                    val = STRING;
                    break;

                case BOOL:
                    syntax_error(tok, "cannot assign an boolean to an string");
                    break;

                case NOTHING:
                    syntax_error(tok, "cannot assign nothing to a string");
                    break;

                default:
                    FATAL("unknown right hand type: %s (%d)", token_to_str(rhs), rhs);
            }
            break;

        case BOOL:
            switch(rhs) {
                case INTEGER:
                    syntax_warning(tok, "assigning an integer to a boolean loses resolution");
                    val = BOOL;
                    break;

                case FLOAT:
                    syntax_warning(tok, "assigning a float to a boolean is always true");
                    val = BOOL;
                    break;

                case STRING:
                    syntax_warning(tok, "assigning a string to a boolean is always true");
                    val = BOOL;
                    break;

                case BOOL:
                    val = BOOL;
                    break;

                case NOTHING:
                    syntax_error(tok, "cannot assign nothing to a bool");
                    break;

                default:
                    FATAL("unknown right hand type: %s (%d)", token_to_str(rhs), rhs);
            }
            break;
        default:
            FATAL("unknown left hand type: %s (%d)", token_to_str(lhs), lhs);
    }

    return val;
}

int check_expression_type(token_t* tok, int left, int right) {

    int val  = -1;
    int oper = tok->type;

    if(left == 0 || right == 0)
        return val;

    switch(oper) {
        // boolean operation always produces a bool
        case OR_OPER:
        case AND_OPER:
        case GT_OPER:
        case EQU_OPER:
        case NEQ_OPER:
        case LTE_OPER:
        case GTE_OPER:
        case LT_OPER:
        case NOT_OPER:
            val = BOOL;
            break;

        // power operation always produces a float
        case POW_OPER:
            val = FLOAT;
            break;

        // unary minus always produces an integer or a float.
        case UNARY_MINUS_OPER:
            switch(right) {
                case INTEGER:
                    val = INTEGER;
                    break;

                case FLOAT:
                    val = FLOAT;
                    break;

                case STRING:
                    syntax_error(tok, "cannot negate a string");
                    break;

                case BOOL:
                    syntax_error(tok, "cannot negate a bool");
                    break;

                case NOTHING:
                    syntax_error(tok, "cannot negate: function has no return value");
                    break;

                default:
                    FATAL("unknown type: %s (%d)", token_to_str(right), right);
            }
            break;

        // type promotions in effect
        case ADD_OPER:
            switch(left) {
                case INTEGER:
                    switch(right) {
                        case INTEGER:
                            val = INTEGER;
                            break;

                        case FLOAT:
                            val = FLOAT;
                            break;

                        case STRING:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        case NOTHING:
                            syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                            break;

                        case BOOL:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case FLOAT:
                    switch(right) {
                        case INTEGER:
                            val = FLOAT;
                            break;

                        case FLOAT:
                            val = FLOAT;
                            break;

                        case STRING:
                            syntax_error(tok, "arithmetic operation not allowed: string and float are incompatible");
                            break;

                        case NOTHING:
                            syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                            break;

                        case BOOL:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case BOOL:
                    switch(right) {
                        case INTEGER:
                            syntax_warning(tok, "boolean operation on integer is always true");
                            val = BOOL;
                            break;

                        case FLOAT:
                            syntax_warning(tok, "boolean operation on float is always true");
                            val = BOOL;
                            break;

                        case STRING:
                            syntax_warning(tok, "boolean operation on string is always true");
                            val = BOOL;
                            break;

                        case NOTHING:
                            syntax_error(tok, "function has no return value");
                            break;

                        case BOOL:
                            val = BOOL;
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                // string + number appends the value to the string
                case STRING:
                    switch(right) {
                        case FLOAT:
                        case STRING:
                        case BOOL:
                        case INTEGER:
                            val = STRING;
                            break;

                        case NOTHING:
                            syntax_error(tok, "function has no return value");
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case NOTHING:
                    syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                    break;

                default:
                    FATAL("unknown left hand type: %s (%d)", token_to_str(left), left);
            }
            break;

        case SUB_OPER:
        case MUL_OPER:
        case DIV_OPER:
        case MOD_OPER:
            switch(left) {
                case INTEGER:
                    switch(right) {
                        case INTEGER:
                            val = INTEGER;
                            break;

                        case FLOAT:
                            val = FLOAT;
                            break;

                        case STRING:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        case NOTHING:
                            syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                            break;

                        case BOOL:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case FLOAT:
                    switch(right) {
                        case INTEGER:
                            val = FLOAT;
                            break;

                        case FLOAT:
                            val = FLOAT;
                            break;

                        case STRING:
                            syntax_error(tok, "arithmetic operation not allowed: string and float are incompatible");
                            break;

                        case NOTHING:
                            syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                            break;

                        case BOOL:
                            syntax_error(tok, "arithmetic operation not allowed: string and integer are incompatible");
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case BOOL:
                    switch(right) {
                        case INTEGER:
                            syntax_warning(tok, "boolean operation on integer is always true");
                            val = BOOL;
                            break;

                        case FLOAT:
                            syntax_warning(tok, "boolean operation on float is always true");
                            val = BOOL;
                            break;

                        case STRING:
                            syntax_warning(tok, "boolean operation on string is always true");
                            val = BOOL;
                            break;

                        case NOTHING:
                            syntax_error(tok, "function has no return value");
                            break;

                        case BOOL:
                            val = BOOL;
                            break;

                        default:
                            FATAL("unknown right hand type: %s (%d)", token_to_str(right), right);
                    }
                    break;

                case NOTHING:
                    syntax_error(tok, "arithmetic operation not allowed: function has no return value");
                    break;

                case STRING:
                    syntax_error(tok, "arithmetic operation not allowed on a string");
                    break;

                default:
                    FATAL("unknown left hand type: %s (%d)", token_to_str(left), left);
            }
            break;

        default:
            FATAL("unknown operator type: %s (%d)", token_to_str(oper), oper);
    }

    return val;
}

int check_expression_cast(ast_type_name_t* type, ast_expression_t* expr) {

    int val   = -1;
    int ttype = type->token->type;
    int ftype = expr->type;

    switch(ttype) {
        case INTEGER:
            switch(ftype) {
                case STRING:
                case INTEGER:
                case FLOAT:
                    val = INTEGER;
                    break;

                case NOTHING:
                    syntax_error(type->token, "cast nothing to integer is not allowed");
                    break;

                case BOOL:
                    break;

                default:
                    FATAL("unknown expression type: %s (%d)", token_to_str(ttype), ttype);
            }
            break;

        case FLOAT:
            switch(ftype) {
                case INTEGER:
                case FLOAT:
                case STRING:
                    val = FLOAT;
                    break;

                case NOTHING:
                    syntax_error(type->token, "cast nothing to float is not allowed");
                    break;

                case BOOL:
                    syntax_error(type->token, "cast boolean to float is not allowed");
                    break;

                default:
                    FATAL("unknown expression type: %s (%d)", token_to_str(ttype), ttype);
            }
            break;

        case STRING:
            switch(ftype) {
                case INTEGER:
                case FLOAT:
                case STRING:
                case BOOL:
                    val = STRING;
                    break;

                case NOTHING:
                    syntax_error(type->token, "cast nothing to string is not allowed");
                    break;

                default:
                    FATAL("unknown expression type: %s (%d)", token_to_str(ttype), ttype);
            }
            break;

        case BOOL:
            switch(ftype) {
                case INTEGER:
                    syntax_warning(type->token, "casting integer to boolean loses resolution");
                    break;

                case FLOAT:
                    syntax_warning(type->token, "casting float to boolean is always true");
                    break;

                case STRING:
                    syntax_warning(type->token, "casting string to boolean is always true");
                    break;

                case NOTHING:
                    syntax_error(type->token, "cast nothing to boolean is not allowed");
                    break;

                case BOOL:
                    val = BOOL;
                    break;

                default:
                    FATAL("unknown expression type: %s (%d)", token_to_str(ttype), ttype);
            }
            break;

        case NOTHING:
            switch(ftype) {
                case INTEGER:
                case FLOAT:
                case STRING:
                case NOTHING:
                case BOOL:
                    syntax_error(type->token, "cast to nothing is not allowed");
                    break;
                default:
                    FATAL("unknown expression type: %s (%d)", token_to_str(ttype), ttype);
            }
            break;

        default:
            FATAL("unknown type name: %s (%d)", token_to_str(ttype), ttype);
    }

    return val;
}
