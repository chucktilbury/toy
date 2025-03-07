/**
 * @file emit_code.c
 *
 * @brief Emit all output code.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include "errors.h"
#include "pointer_list.h"
#include "ast.h"
#include "parser.h"
#include "fileio.h"

// #define EMIT_TRACE

#ifdef EMIT_TRACE
#define USE_TRACE
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif
#include "trace.h"

// defined in parser.y
const char* token_to_str(int);

typedef enum {
    IS_IDLE,
    IN_LIST,
    IN_EXPR,
} emit_state_t;

static bool is_last     = false;
static bool is_data_def = false;
static bool in_loop     = false;
static bool in_expr     = false;
static bool main_seen   = false;

#define EMIT(...) append_string_buffer_fmt(buffer, __VA_ARGS__);

#if 0
#define LINE EMIT("\n#line %d \"%s\"\n\n", node->node.line, node->node.fname);
#else
#define LINE
#endif

static string_buffer_t* buffer;

static inline void emit_program(ast_program_t* node);
static inline void emit_program_item_list(ast_program_item_list_t* node);
static inline void emit_program_item(ast_program_item_t* node);
static inline void emit_start_block(ast_start_block_t* node);
static inline void emit_type_name(ast_type_name_t* node);
static inline void emit_formatted_string(ast_formatted_string_t* node);
static inline void emit_data_declaration(ast_data_declaration_t* node);
static inline void emit_data_declaration_list(ast_data_declaration_list_t* node);
static inline void emit_data_definition(ast_data_definition_t* node);
static inline void emit_func_name(ast_func_name_t* node);
static inline void emit_func_definition(ast_func_definition_t* node);
static inline void emit_func_params(ast_func_params_t* node);
static inline void emit_func_body(ast_func_body_t* node);
static inline void emit_func_body_list(ast_func_body_list_t* node);
static inline void emit_loop_body(ast_loop_body_t* node);
static inline void emit_loop_body_list(ast_loop_body_list_t* node);
static inline void emit_loop_body_diffs(ast_loop_body_diffs_t* node);
static inline void emit_loop_body_elem(ast_loop_body_elem_t* node);
static inline void emit_func_body_elem(ast_func_body_elem_t* node);
static inline void emit_inline_statement(ast_inline_statement_t* node);
static inline void emit_return_statement(ast_return_statement_t* node);
static inline void emit_if_clause(ast_if_clause_t* node);
static inline void emit_ifelse_statement(ast_ifelse_statement_t* node);
static inline void emit_else_segment(ast_else_segment_t* node);
static inline void emit_final_else_segment(ast_final_else_segment_t* node);
static inline void emit_else_clause_list(ast_else_clause_list_t* node);
static inline void emit_else_clause(ast_else_clause_t* node);
static inline void emit_while_clause(ast_while_clause_t* node);
static inline void emit_while_statement(ast_while_statement_t* node);
static inline void emit_do_statement(ast_do_statement_t* node);
static inline void emit_assignment_right(ast_assignment_right_t* node);
static inline void emit_assignment(ast_assignment_t* node);
static inline void emit_raw_value(ast_raw_value_t* node);
static inline void emit_expr_primary(ast_expr_primary_t* node);
static inline void emit_func_reference(ast_func_reference_t* node);
static inline void emit_expression(ast_expression_t* node);
static inline void emit_expression_list(ast_expression_list_t* node);
static inline void emit_expression_list_param(ast_expression_list_param_t* node);
static inline void emit_expression_param(ast_expression_param_t* node);

/*
 * program
 *    : program_item_list
 *    ;
 */
static inline void emit_program(ast_program_t* node) {

    ENTER;

    time_t t = time(NULL);

    EMIT("//------------------------\n");
    EMIT("//---- generated code ----\n");
    EMIT("//----- DO NOT EDIT ------\n");
    EMIT("//------------------------\n");
    EMIT("// source: %s\n", get_base_file_name());
    EMIT("// date:   %s", ctime(&t));
    EMIT("// user:   %s\n\n", getenv("USER"));

    EMIT("#include <stdio.h>\n");
    EMIT("#include <stdlib.h>\n");
    EMIT("#include <stdint.h>\n");
    EMIT("#include <stdbool.h>\n");
    EMIT("#include <string.h>\n\n");

    EMIT("#include \"runtime.h\"\n\n");

    EMIT("// intrinsic types for TOY code\n");
    EMIT("typedef double TOY_FLOAT;\n");
    EMIT("typedef int64_t TOY_INTEGER;\n");
    EMIT("typedef void TOY_NOTHING;\n");
    EMIT("typedef bool TOY_BOOL;\n");
    EMIT("typedef char* TOY_STRING;\n\n");

    EMIT("//---------------------------\n");
    EMIT("//----- begin user code -----\n");
    EMIT("//---------------------------\n\n");

    emit_program_item_list(node->program_item_list);

    EMIT("\n\n//---------------------------\n");
    EMIT("//-- end of generated code --\n");
    EMIT("//---------------------------\n\n");

    RETURN();
}

/*
 * program_item_list
 *    : program_item
 *    | program_item_list program_item
 *    ;
 */
static inline void emit_program_item_list(ast_program_item_list_t* node) {

    ENTER;

    int mark = 0;
    ast_program_item_t* pi;

    while(NULL != (pi = iterate_pointer_list(node->list, &mark))) {
        LINE;
        emit_program_item(pi);
    }

    RETURN();
}

/*
 * program_item
 *    : data_definition
 *    | func_definition
 *    | start_block
 *    ;
 */
static inline void emit_program_item(ast_program_item_t* node) {

    ENTER;

    ast_type_t type = node->nterm->type;

    switch(type) {
        case AST_DATA_DECLARATION:
            emit_data_declaration((ast_data_declaration_t*)node->nterm);
            break;
        case AST_FUNC_DEFINITION:
            emit_func_definition((ast_func_definition_t*)node->nterm);
            break;
        case AST_START_BLOCK:
            emit_start_block((ast_start_block_t*)node->nterm);
            break;
        case AST_IMPORT_STATEMENT:
            // do nothing
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * start_block
 *    : START func_body
 *    ;
 */
static inline void emit_start_block(ast_start_block_t* node) {

    ENTER;

    if(false == main_seen) {
        EMIT("\n\nint main(int argc, char** argv, char**env){");
        EMIT("runtime_main_init(argc, argv, env);");
        emit_func_body(node->func_body);
        EMIT("runtime_main_uninit();return runtime_error_number;}");
        main_seen = true;
    }
    else
        node_syntax((ast_node_t*)node, "only one \"start {}\" entry is allowed in a compile unit.");

    RETURN();
}

/*
 * type_name
 *    : INTEGER
 *    | FLOAT
 *    | STRING
 *    | LIST
 *    | HASH
 *    ;
 */
static inline void emit_type_name(ast_type_name_t* node) {

    ENTER;

    EMIT("TOY_%s ", token_to_str(node->token->type));

    RETURN();
}

/*
 * formatted_string
 *    : STRING_LIT expression_list_param
 *    | STRING_LIT
 *    ;
 */
static inline void emit_formatted_string(ast_formatted_string_t* node) {

    ENTER;

    EMIT("runtime_string_format(\"%s\"", node->STRING_LIT->raw);
    if(node->expression_list_param != NULL) {
        in_loop = true;
        in_expr = true;
        EMIT(", ");
        emit_expression_list(node->expression_list_param->expression_list);
        in_loop = false;
        in_expr = false;
    }
    EMIT(")");

    RETURN();
}

/*
 * Generate a data definition symbol.
 *
 * data_declaration
 *    : type_name IDENTIFIER
 *    | CONST type_name IDENTIFIER
 *    ;
 */
static inline void emit_data_declaration(ast_data_declaration_t* node) {

    ENTER;

    if(node->is_const)
        EMIT("const ");
    emit_type_name(node->type_name);
    EMIT("TOY_%s", node->IDENTIFIER->raw);

    if(in_loop) {
        if(!is_last)
            EMIT(", ");
    }
    else if(!is_data_def)
        EMIT("; ");

    RETURN();
}

/*
 * data_declaration_list
 *    : data_declaration
 *    | data_declaration_list ',' data_declaration
 *    ;
 */
static inline void emit_data_declaration_list(ast_data_declaration_list_t* node) {

    ENTER;

    is_last = false;
    in_loop = true;
    ast_data_declaration_t* dd;
    int mark = 0;

    while(NULL != (dd = iterate_pointer_list(node->list, &mark))) {
        if(mark >= node->list->len) {
            is_last = true;
        }
        emit_data_declaration(dd);
    }

    in_loop = false;

    RETURN();
}

/*
 * data_definition
 *    : data_declaration
 *    | data_declaration '=' expression
 *    ;
 */
static inline void emit_data_definition(ast_data_definition_t* node) {

    ENTER;

    is_data_def = true;
    emit_data_declaration(node->data_declaration);
    EMIT(" = (");
    in_expr = true;
    emit_expression(node->expression);
    in_expr = false;
    EMIT("); ");
    is_data_def = false;

    RETURN();
}

/*
 * Generate a function name symbol, but the decorated symbol name
 * cannot be generated until the parameters have been parsed.
 *
 * func_name
 *    : type_name IDENTIFIER
 *    | ITERATOR type_name IDENTIFIER
 *    | NOTHING IDENTIFIER
 *    ;
 */
static inline void emit_func_name(ast_func_name_t* node) {

    ENTER;

    if(node->type_name != NULL) {
        EMIT("\n\n")
        emit_type_name(node->type_name);
    }
    else
        EMIT("void");

    EMIT("FUNC_%s", node->IDENTIFIER->raw);

    RETURN();
}

/*
 * Add the parameters in their own symbol context and finish generating
 * the decorated function name.
 *
 * func_definition
 *    : func_name func_params func_body
 *    ;
 */
static inline void emit_func_definition(ast_func_definition_t* node) {

    ENTER;

    emit_func_name(node->func_name);
    emit_func_params(node->func_params);

    EMIT("{runtime_gc_node_t* runtime_gc_node = runtime_gc_begin();");
    if(node->func_name->type_name->token->type != NOTHING)
        EMIT("TOY_%s runtime_return_value;", token_to_str(node->func_name->type_name->token->type));

        emit_func_body(node->func_body);

    EMIT("runtime_function_return_label:");
    if(node->func_name->type_name->token->type != NOTHING) {
        EMIT("runtime_gc_end(runtime_gc_node, runtime_return_value);");
        EMIT("return runtime_return_value;}");
    }
    else {
        EMIT("runtime_gc_end(runtime_gc_node, NULL);");
        EMIT("return;}");
    }

    RETURN();
}

/*
 * func_params
 *    : '(' data_declaration_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void emit_func_params(ast_func_params_t* node) {

    ENTER;

    EMIT("(");
    in_loop = true;
    emit_data_declaration_list(node->data_declaration_list);
    in_loop = false;
    EMIT(")");

    RETURN();
}

/*
 * func_body
 *    : '{' func_body_list '}'
 *    ;
 */
static inline void emit_func_body(ast_func_body_t* node) {

    ENTER;

    EMIT("{");
    emit_func_body_list(node->func_body_list);
    EMIT("}");

    RETURN();
}

/*
 * func_body_list
 *    : func_body_elem
 *    | func_body_list func_body_elem
 *    ;
 */
static inline void emit_func_body_list(ast_func_body_list_t* node) {

    ENTER;

    int mark = 0;
    ast_func_body_elem_t* be;

    while(NULL != (be = iterate_pointer_list(node->list, &mark))) {
        LINE;
        emit_func_body_elem(be);
    }

    RETURN();
}

/*
 * loop_body
 *    : '{' '}'
 *    | '{' loop_body_list '}'
 *    ;
 */
static inline void emit_loop_body(ast_loop_body_t* node) {

    ENTER;

    EMIT("{ ");
    emit_loop_body_list(node->loop_body_list);
    EMIT("} ");

    RETURN();
}

/*
 * loop_body_list
 *    : loop_body_elem
 *    | loop_body_list loop_body_elem
 *    ;
 */
static inline void emit_loop_body_list(ast_loop_body_list_t* node) {

    ENTER;

    int mark = 0;
    ast_loop_body_elem_t* lbe;

    while(NULL != (lbe = iterate_pointer_list(node->list, &mark))) {
        LINE;
        emit_loop_body_elem(lbe);
    }

    RETURN();
}

/*
 * loop_body_diffs
 *    : BREAK
 *    | CONTINUE
 *    ;
 */
static inline void emit_loop_body_diffs(ast_loop_body_diffs_t* node) {

    ENTER;

    switch(node->type) {
        case BREAK:
            EMIT("break;");
            break;
        case CONTINUE:
            EMIT("continue;");
            break;
        default:
            FATAL("unknown node type: %s (%d)", token_to_str(node->type), node->type);
    }

    RETURN();
}

/*
 * loop_body_elem
 *    : func_body_elem
 *    | loop_body_diffs
 *    ;
 */
static inline void emit_loop_body_elem(ast_loop_body_elem_t* node) {

    ENTER;

    if(node->nterm->type == AST_LOOP_BODY_DIFFS)
        emit_loop_body_diffs((ast_loop_body_diffs_t*)node->nterm);
    else if(node->nterm->type == AST_FUNC_BODY_ELEM)
        emit_func_body_elem((ast_func_body_elem_t*)node->nterm);
    else
        FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);

    RETURN();
}

/*
 * func_body_elem
 *    : data_definition
 *    | func_reference
 *    | assignment
 *    | while_statement
 *    | do_statement
 *    | for_statement
 *    | ifelse_statement
 *    | return_statement
 *    | func_body
 *    ;
 */
static inline void emit_func_body_elem(ast_func_body_elem_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_DATA_DEFINITION:
            emit_data_definition((ast_data_definition_t*)node->nterm);
            break;
        case AST_FUNC_REFERENCE:
            emit_func_reference((ast_func_reference_t*)node->nterm);
            break;
        case AST_ASSIGNMENT:
            emit_assignment((ast_assignment_t*)node->nterm);
            break;
        case AST_WHILE_STATEMENT:
            emit_while_statement((ast_while_statement_t*)node->nterm);
            break;
        case AST_DO_STATEMENT:
            emit_do_statement((ast_do_statement_t*)node->nterm);
            break;
        case AST_IFELSE_STATEMENT:
            emit_ifelse_statement((ast_ifelse_statement_t*)node->nterm);
            break;
        case AST_RETURN_STATEMENT:
            emit_return_statement((ast_return_statement_t*)node->nterm);
            break;
        case AST_INLINE_STATEMENT:
            emit_inline_statement((ast_inline_statement_t*)node->nterm);
            break;
        case AST_FUNC_BODY:
            emit_func_body((ast_func_body_t*)node->nterm);
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * inline_statement
 *    : INLINE
 *    ;
 */
static inline void emit_inline_statement(ast_inline_statement_t* node) {

    ENTER;

    append_string_buffer_fmt(buffer, node->token->raw);

    RETURN();
}

/*
 * return_statement
 *    : RETURN expression_param
 *    | RETURN
 *    ;
 */
static inline void emit_return_statement(ast_return_statement_t* node) {

    ENTER;


//    EMIT("return ");
    EMIT("{");
    if(node->expression_param != NULL) {
        EMIT("runtime_return_value=");
        emit_expression_param(node->expression_param);
        EMIT(";");
    }
    EMIT("goto runtime_function_return_label;}");

    RETURN();
}


/*
 * if_clause
 *    : IF '(' expression ')' func_body
 *    ;
 */
static inline void emit_if_clause(ast_if_clause_t* node) {

    ENTER;

    EMIT("if(");
    emit_expression(node->expression);
    EMIT(")");
    emit_func_body(node->func_body);

    RETURN();
}

/*
 * ifelse_statement
 *    : if_clause
 *    | if_clause else_clause
 *    ;
 */
static inline void emit_ifelse_statement(ast_ifelse_statement_t* node) {

    ENTER;

    emit_if_clause(node->if_clause);
    if(node->else_clause != NULL)
        emit_else_clause(node->else_clause);

    RETURN();
}

/*
 * else_segment
 *    : ELSE '(' expression ')' func_body
 *    ;
 */
static inline void emit_else_segment(ast_else_segment_t* node) {

    ENTER;

    EMIT("else if(");
    emit_expression(node->expression);
    EMIT(")");
    emit_func_body(node->func_body);

    RETURN();
}

/*
 * final_else_segment
 *    : ELSE '(' ')' func_body
 *    | ELSE func_body
 *    ;
 */
static inline void emit_final_else_segment(ast_final_else_segment_t* node) {

    ENTER;

    EMIT("else");
    emit_func_body(node->func_body);

    RETURN();
}

/*
 * else_clause_list
 *    : else_segment
 *    | else_clause_list else_segment
 *    ;
 */
static inline void emit_else_clause_list(ast_else_clause_list_t* node) {

    ENTER;

    int mark = 0;
    ast_else_segment_t* es;

    while(NULL != (es = iterate_pointer_list(node->list, &mark)))
        emit_else_segment(es);

    RETURN();
}

/*
 * else_clause
 *    : else_clause_list
 *    | else_clause_list final_else_segment
 *    | final_else_segment
 *    ;
 */
static inline void emit_else_clause(ast_else_clause_t* node) {

    ENTER;

    if(node->else_clause_list != NULL)
        emit_else_clause_list(node->else_clause_list);

    if(node->final_else_segment != NULL)
        emit_final_else_segment(node->final_else_segment);

    RETURN();
}

/*
 * while_clause
 *    : WHILE expression_param
 *    | WHILE
 *    ;
 */
static inline void emit_while_clause(ast_while_clause_t* node) {

    ENTER;

    EMIT("while(");
    emit_expression_param(node->expression_param);
    EMIT(")");


    RETURN();
}

/*
 * while_statement
 *    : while_clause loop_body
 *    ;
 */
static inline void emit_while_statement(ast_while_statement_t* node) {

    ENTER;

    emit_while_clause(node->while_clause);
    emit_loop_body(node->loop_body);

    RETURN();
}

/*
 * do_statement
 *    : DO loop_body while_clause
 *    ;
 */
static inline void emit_do_statement(ast_do_statement_t* node) {

    ENTER;

    EMIT("do");
    emit_loop_body(node->loop_body);
    emit_while_clause(node->while_clause);

    RETURN();
}

/*
 * assignment_right
 *    : expression
 *    | type_name ':' expression
 *    ;
 */
static inline void emit_assignment_right(ast_assignment_right_t* node) {

    ENTER;


    if(node->type_name != NULL) {
        EMIT("(");
        emit_type_name(node->type_name);
        EMIT(")");
    }

    emit_expression(node->expression);
    EMIT(";");

    RETURN();
}


/*
 * assignment
 *    : IDENTIFIER '=' assignment_right
 *    ;
 */
static inline void emit_assignment(ast_assignment_t* node) {

    ENTER;

    // emit the indetifier
    EMIT("TOY_%s = ", node->IDENTIFIER->raw);
    emit_assignment_right(node->assignment_right);

    RETURN();
}

/*
 * references only.
 *
 * raw_value
 *    : IDENTIFIER
 *    | INTEGER_LIT
 *    | FLOAT_LIT
 *    ;
 */
static inline void emit_raw_value(ast_raw_value_t* node) {

    ENTER;

    switch(node->token->type) {
        case IDENTIFIER:
            EMIT("TOY_%s", node->token->raw);
            break;
        case INTEGER_LIT:
            EMIT("%ld", strtol(node->token->raw, NULL, 10));
            break;
        case FLOAT_LIT:
            EMIT("%0.5lf", strtod(node->token->raw, NULL));
            break;
        default:
            FATAL("unknown token type: %s (%d)", token_to_str(node->token->type), node->token->type);
    }

    RETURN();
}

/*
 * expr_primary
 *    : raw_value
 *    | formatted_string
 *    ;
 */
static inline void emit_expr_primary(ast_expr_primary_t* node) {

    ENTER;

    switch(node->nterm->type) {
        case AST_RAW_VALUE:
            emit_raw_value((ast_raw_value_t*)node->nterm);
            break;
        case AST_FORMATTED_STRING:
            emit_formatted_string((ast_formatted_string_t*)node->nterm);
            break;
        case AST_FUNC_REFERENCE:
            emit_func_reference((ast_func_reference_t*)node->nterm);
            break;
        default:
            FATAL("unknown node type: %s (%d)", node_type_to_name(node->nterm), node->nterm->type);
    }

    RETURN();
}

/*
 * func_reference
 *    : IDENTIFIER expression_list_param
 *    ;
 */
static inline void emit_func_reference(ast_func_reference_t* node) {

    ENTER;

    EMIT("FUNC_%s", node->IDENTIFIER->raw);
    emit_expression_list_param(node->expression_list_param);
    if(!in_loop && !in_expr)
        EMIT(";");

    RETURN();
}

/*
 * expression
 *    : expr_primary
 *    | expression ADD_OPER expression
 *    | expression SUB_OPER expression
 *    | expression MUL_OPER expression
 *    | expression DIV_OPER expression
 *    | expression MOD_OPER expression
 *    | expression POW_OPER expression
 *    | expression GT_OPER expression
 *    | expression LT_OPER expression
 *    | expression EQU_OPER expression
 *    | expression NEQ_OPER expression
 *    | expression LTE_OPER expression
 *    | expression GTE_OPER expression
 *    | expression OR_OPER expression
 *    | expression AND_OPER expression
 *    | NOT_OPER expression %prec UNARY
 *    | SUB_OPER expression %prec UNARY
 *    | '(' expression ')'
 *    ;
 */
static inline void emit_expression(ast_expression_t* node) {

    ENTER;

    if(node->expr_primary != NULL) {
        emit_expr_primary(node->expr_primary);
    }
    else if(node->expression != NULL) {
        EMIT("(");
        emit_expression(node->expression);
        EMIT(")");
    }
    else if(node->oper->type == NOT_OPER) {
        EMIT("!");
        emit_expression(node->right);
    }
    else if(node->oper->type == UNARY_MINUS_OPER) {
        EMIT("-");
        emit_expression(node->right);
    }
    else if(node->oper->type == POW_OPER) {
        EMIT("pow(");
        emit_expression(node->left);
        EMIT(", ");
        emit_expression(node->right);
        EMIT(")");
    }
    else {
        emit_expression(node->left);
        switch(node->oper->type) {
            case EQU_OPER:
                EMIT("==");
                break;
            case NEQ_OPER:
                EMIT("!=");
                break;
            case LTE_OPER:
                EMIT("<=");
                break;
            case GTE_OPER:
                EMIT(">=");
                break;
            case LT_OPER:
                EMIT("<");
                break;
            case OR_OPER:
                EMIT("||");
                break;
            case AND_OPER:
                EMIT("&&");
                break;
            case GT_OPER:
                EMIT(">");
                break;
            case ADD_OPER:
                EMIT("+");
                break;
            case SUB_OPER:
                EMIT("-");
                break;
            case MUL_OPER:
                EMIT("*");
                break;
            case DIV_OPER:
                EMIT("/");
                break;
            case MOD_OPER:
                EMIT("%");
                break;
            default:
                FATAL("unknown operator: \"%s\" (%d)", node->oper->raw, node->oper->type);
        }
        emit_expression(node->right);
    }

    RETURN();
}

/*
 * expression_list
 *    : expression
 *    | expression_list ',' expression
 *    ;
 */
static inline void emit_expression_list(ast_expression_list_t* node) {

    ENTER;

    int mark = 0;
    ast_expression_t* ex;

    while(NULL != (ex = iterate_pointer_list(node->list, &mark))) {
        emit_expression(ex);
        if(mark < node->list->len)
            EMIT(", ");
    }

    RETURN();
}

/*
 * expression_list_param
 *    : '(' expression_list ')'
 *    | '(' ')'
 *    ;
 */
static inline void
        emit_expression_list_param(ast_expression_list_param_t* node) {

    ENTER;

    EMIT("(");
    in_loop = true;
    emit_expression_list(node->expression_list);
    in_loop = false;
    EMIT(")");

    RETURN();
}

/*
 * expression_param
 *    : '(' expression ')'
 *    | '(' ')'
 *    ;
 */
static inline void emit_expression_param(ast_expression_param_t* node) {

    ENTER;

    EMIT("(");
    in_expr = true;
    emit_expression(node->expression);
    in_expr = false;
    EMIT(")");

    RETURN();
}

/******************************************************************************
 * PUBLIC INTERFACE
 *****************************************************************************/
#include "string_buffer.h"

void emit_code(const char* fname) {

    SEPARATOR;
    ENTER;

    buffer = create_string_buffer(NULL);

    if(!get_errors()) {
        emit_program(get_program());
    }

    write_string_buffer(buffer, fname);

    RETURN();
}
