#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "errors.h"
#include "fileio.h"
#include "hash.h"
#include "memory.h"
#include "pointer_list.h"
#include "ast.h"
#include "parser.h"

// defined at the end of parser.y
const char* tokenToStr(int tok);

// global used as the return value of the parser.
ast_node_t* root_node = NULL;

#define PRE_STATE         \
    do {                  \
        if(pre != NULL)   \
            (*pre)(node); \
    } while(0)

#define POST_STATE         \
    do {                   \
        if(post != NULL)   \
            (*post)(node); \
    } while(0)

#define FIND_ATTRIB(name, ptr) find_hashtable(node->table, #name, &ptr)

#define GET_ATTRIB(name, ptr)                                        \
    do {                                                             \
        if(!FIND_ATTRIB(name, ptr))                                  \
            FATAL("improper node format: \"%s\" not found", #name);  \
        else if(ptr == NULL)                                         \
            FATAL("improper node format: \"%s\" is invalid", #name); \
    } while(0)

#define TRAVERSE_OBJ(name)      \
    do {                        \
        void* data;             \
        GET_ATTRIB(name, data); \
        name(data, pre, post);  \
    } while(0)

#define TRAVERSE_OPTION_OBJ(name)      \
    do {                               \
        void* data;                    \
        if(FIND_ATTRIB(name, data)) {  \
            if(data != NULL)           \
                name(data, pre, post); \
        }                              \
    } while(0)

#define TRAVERSE_OBJ_LIST(name)                                     \
    do {                                                            \
        void* data;                                                 \
        GET_ATTRIB(list, data);                                     \
        ast_node_t* item;                                           \
        int mark = 0;                                               \
        while(NULL != (item = iterate_pointer_list(data, &mark))) { \
            name(item, pre, post);                                  \
        }                                                           \
    } while(0)

#define TRACE_AST_STATE

#ifdef TRACE_AST_STATE
static int depth      = 0;
static int num_states = 0;
#define DINC 2
#define TRACE(...)                                \
    do {                                          \
        fprintf(stdout, "%*sTRACE: ", depth, ""); \
        fprintf(stdout, __VA_ARGS__);             \
        fprintf(stdout, "\n");                    \
    } while(0)

#define TRACE_TOKEN(name)                                \
    do {                                                 \
        void* data;                                      \
        GET_ATTRIB(name, data);                          \
        TRACE("%s: %s", #name, tokenToStr(*(int*)data)); \
    } while(0)

#define TRACE_STRING(name)                         \
    do {                                           \
        void* data;                                \
        GET_ATTRIB(name, data);                    \
        TRACE("%s: %s", #name, (const char*)data); \
    } while(0)

#define TRACE_FLAG(name)                                          \
    do {                                                          \
        void* data;                                               \
        GET_ATTRIB(name, data);                                   \
        TRACE("%s: %s", #name, *((int*)data) ? "TRUE" : "FALSE"); \
    } while(0)

#define TRACE_INT(name)                      \
    do {                                     \
        void* data;                          \
        GET_ATTRIB(name, data);              \
        TRACE("%s: %d", #name, *(int*)data); \
    } while(0)

#define ENTER(name)                                                                            \
    do {                                                                                       \
        ASSERT(node != NULL, "");                                                              \
        ASSERT(node->type == name, "node type = %s (%d)", node_type_to_str(node), node->type); \
        fprintf(stdout, "%*sENTER: %s\n", depth, "", __func__);                                \
        depth += DINC;                                                                         \
        num_states++;                                                                          \
        PRE_STATE;                                                                             \
    } while(0)

#define RETURN                                                   \
    do {                                                         \
        depth -= DINC;                                           \
        fprintf(stdout, "%*sRETURN: %s\n", depth, "", __func__); \
        POST_STATE;                                              \
        return;                                                  \
    } while(0)

#define START                                                     \
    do {                                                          \
        depth      = 0;                                           \
        num_states = 0;                                           \
        fprintf(stdout, "\n%*sSTART: %s\n", depth, "", __func__); \
        depth += DINC;                                            \
    } while(0)

#define FINISH                                                                                     \
    do {                                                                                           \
        depth -= DINC;                                                                             \
        fprintf(stdout, "%*sFINISH: number of states: %d: %s\n", depth, "", num_states, __func__); \
        return;                                                                                    \
    } while(0)
#else
#define TRACE(...)
#define TRACE_TOKEN(name)
#define TRACE_STRING(name)
#define TRACE_FLAG(name)
#define TRACE_INT(name)
#define ENTER      \
    do {           \
        PRE_STATE; \
    } while(0)

#define RETURN      \
    do {            \
        POST_STATE; \
        return;     \
    } while(0)

#define START

#define FINISH RETURN

#endif

static void program(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void program_item_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void program_item(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void type_name(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void formatted_string(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void data_declaration(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void data_declaration_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void data_definition(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_definition(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_params(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_body(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_name(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_body_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void loop_body(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void loop_body_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void loop_body_diffs(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void loop_body_elem(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_body_elem(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void trace_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void print_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void exit_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void return_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void tryexcept_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void try_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void except_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void except_clause_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void final_except_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void except_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void raise_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void if_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void ifelse_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void else_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void final_else_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void else_clause_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void else_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void while_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void while_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void do_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void assignment_right(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void assignment_left(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void assignment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void raw_value(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void expr_primary(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void list_reference(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void list_ref_value(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void list_ref_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void list_ref_param_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void func_reference(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void expression(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void expression_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void expression_list_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void expression_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void dict_init(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void list_init(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void dict_init_item(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));
static void dict_init_item_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*));


static void program(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_PROGRAM);

    TRAVERSE_OBJ(program_item_list);

    RETURN;
}

static void program_item_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_PROGRAM_ITEM_LIST);

    TRAVERSE_OBJ_LIST(program_item);

    RETURN;
}

static void program_item(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_PROGRAM_ITEM);

    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    ast_type_t type = *(ast_type_t*)ptr;

    GET_ATTRIB(node, ptr);

    switch(type) {
        case AST_DATA_DEFINITION:
            data_definition(ptr, pre, post);
            break;
        case AST_FUNC_DEFINITION:
            func_definition(ptr, pre, post);
            break;
        case AST_FUNC_BODY:
            func_body(ptr, pre, post);
            break;
        case AST_IMPORT_STATEMENT:
            // do nothing
            break;
        default:
            FATAL("unknown node type %d", type);
    }
    RETURN;
}

static void type_name(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_TYPE_NAME);

    TRACE_TOKEN(type);

    RETURN;
}

static void formatted_string(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FORMATTED_STRING);

    TRAVERSE_OPTION_OBJ(expression_list_param);

    TRACE_STRING(STRING_LIT);

    RETURN;
}


static void data_declaration(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DATA_DECLARATION);

    TRAVERSE_OBJ(type_name);
    TRACE_STRING(IDENTIFIER);
    TRACE_FLAG(is_const);

    RETURN;
}

static void data_declaration_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DATA_DECLARATION_LIST);

    TRAVERSE_OBJ_LIST(data_declaration);

    RETURN;
}

static void data_definition(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DATA_DEFINITION);

    void* type;
    GET_ATTRIB(type, type);
    TRACE("type = %d", *(int*)type);
    TRACE_FLAG(is_init);

    switch(*(int*)type) {
        case 0:
            TRAVERSE_OBJ(data_declaration);
            break;
        case 1:
            TRAVERSE_OBJ(data_declaration);
            TRAVERSE_OBJ(expression);
            break;
        case 2:
            TRAVERSE_OBJ(data_declaration);
            TRAVERSE_OBJ(list_init);
            break;
        case 3:
            TRAVERSE_OBJ(data_declaration);
            TRAVERSE_OBJ(dict_init);
            break;
        default:
            FATAL("unknown type: %d", *(int*)type);
    }

    RETURN;
}

static void func_definition(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_DEFINITION);

    TRAVERSE_OBJ(func_name);
    TRAVERSE_OBJ(func_params);
    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void func_params(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_PARAMS);

    TRAVERSE_OBJ(data_declaration_list);

    RETURN;
}

static void func_body(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_BODY);

    TRAVERSE_OBJ(func_body_list);

    RETURN;
}

static void func_name(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_NAME);

    TRAVERSE_OBJ(type_name);
    TRACE_STRING(IDENTIFIER);

    RETURN;
}

static void func_body_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_BODY_LIST);

    TRAVERSE_OBJ_LIST(func_body_elem);

    RETURN;
}

static void loop_body(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LOOP_BODY);

    TRAVERSE_OPTION_OBJ(loop_body_list);

    RETURN;
}

static void loop_body_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LOOP_BODY_LIST);

    TRAVERSE_OBJ_LIST(loop_body_elem);

    RETURN;
}

static void loop_body_diffs(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LOOP_BODY_DIFFS);

    TRACE_TOKEN(type);

    RETURN;
}

static void loop_body_elem(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LOOP_BODY_ELEM);

    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    ast_type_t type = *(ast_type_t*)ptr;
    GET_ATTRIB(node, ptr);

    switch(type) {
        case AST_FUNC_BODY_ELEM:
            func_body_elem(ptr, pre, post);
            break;
        case AST_LOOP_BODY_DIFFS:
            loop_body_diffs(ptr, pre, post);
            break;
        default:
            FATAL("unknown node type %d", type);
    }

    RETURN;
}

static void func_body_elem(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_BODY_ELEM);

    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    ast_type_t type = *(ast_type_t*)ptr;
    GET_ATTRIB(node, ptr);

    switch(type) {
        case AST_DATA_DEFINITION:
            data_definition(ptr, pre, post);
            break;
        case AST_FUNC_REFERENCE:
            func_reference(ptr, pre, post);
            break;
        case AST_ASSIGNMENT:
            assignment(ptr, pre, post);
            break;
        case AST_WHILE_STATEMENT:
            while_statement(ptr, pre, post);
            break;
        case AST_DO_STATEMENT:
            do_statement(ptr, pre, post);
            break;
        case AST_IFELSE_STATEMENT:
            ifelse_statement(ptr, pre, post);
            break;
        case AST_TRYEXCEPT_STATEMENT:
            tryexcept_statement(ptr, pre, post);
            break;
        case AST_RAISE_STATEMENT:
            raise_statement(ptr, pre, post);
            break;
        case AST_RETURN_STATEMENT:
            return_statement(ptr, pre, post);
            break;
        case AST_EXIT_STATEMENT:
            exit_statement(ptr, pre, post);
            break;
        case AST_PRINT_STATEMENT:
            print_statement(ptr, pre, post);
            break;
        case AST_TRACE_STATEMENT:
            trace_statement(ptr, pre, post);
            break;
        default:
            FATAL("unknown node type %d", type);
    }

    RETURN;
}

static void trace_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_TRACE_STATEMENT);

    TRAVERSE_OPTION_OBJ(expression_list_param);

    RETURN;
}

static void print_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_PRINT_STATEMENT);

    TRAVERSE_OPTION_OBJ(expression_list_param);

    RETURN;
}

static void exit_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXIT_STATEMENT);

    TRAVERSE_OPTION_OBJ(expression_param);

    RETURN;
}

static void return_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_RETURN_STATEMENT);

    TRAVERSE_OPTION_OBJ(expression_param);

    RETURN;
}

static void tryexcept_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_TRYEXCEPT_STATEMENT);

    TRAVERSE_OBJ(try_clause);
    TRAVERSE_OBJ(except_clause);

    RETURN;
}

static void try_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_TRY_CLAUSE);

    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void except_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXCEPT_SEGMENT);

    TRACE_STRING("ename");
    TRACE_STRING("mname");
    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void except_clause_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXCEPT_CLAUSE_LIST);

    TRAVERSE_OBJ_LIST(except_segment);

    RETURN;
}

static void final_except_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FINAL_EXCEPT_CLAUSE);

    TRACE_STRING("mname");
    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void except_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXCEPT_CLAUSE);

    TRAVERSE_OPTION_OBJ(except_clause_list);
    TRAVERSE_OPTION_OBJ(final_except_clause);

    RETURN;
}

static void raise_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_RAISE_STATEMENT);

    TRAVERSE_OBJ(formatted_string);
    TRACE_STRING("ename");

    RETURN;
}

static void if_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_IF_CLAUSE);

    TRAVERSE_OBJ(expression);
    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void ifelse_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_IFELSE_STATEMENT);

    TRAVERSE_OBJ(if_clause);
    TRAVERSE_OPTION_OBJ(else_clause);

    RETURN;
}

static void else_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ELSE_SEGMENT);

    TRAVERSE_OBJ(expression);
    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void final_else_segment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FINAL_ELSE_SEGMENT);

    TRAVERSE_OBJ(func_body);

    RETURN;
}

static void else_clause_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ELSE_CLAUSE_LIST);

    TRAVERSE_OBJ_LIST(else_segment);

    RETURN;
}

static void else_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ELSE_CLAUSE);

    TRAVERSE_OPTION_OBJ(else_clause_list);
    TRAVERSE_OPTION_OBJ(final_else_segment);

    RETURN;
}

static void while_clause(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_WHILE_CLAUSE);

    TRAVERSE_OPTION_OBJ(expression_param);

    RETURN;
}

static void while_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_WHILE_STATEMENT);

    TRAVERSE_OBJ(loop_body);
    TRAVERSE_OBJ(while_clause);

    RETURN;
}

static void do_statement(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DO_STATEMENT);

    TRAVERSE_OBJ(loop_body);
    TRAVERSE_OBJ(while_clause);

    RETURN;
}

static void assignment_right(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ASSIGNMENT_RIGHT);

    void* type;

    GET_ATTRIB(type, type);
    switch(*(int*)type) {
        case 0:
            TRAVERSE_OBJ(expression);
            break;
        case 1:
            TRAVERSE_OBJ(expression);
            TRAVERSE_OBJ(type_name);
            break;
        case 2:
            TRAVERSE_OBJ(list_init);
            break;
        case 3:
            TRAVERSE_OBJ(dict_init);
            break;
        default:
            FATAL("unknown object type: %d", *(int*)type);
    }

    RETURN;
}

static void assignment_left(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ASSIGNMENT_LEFT);

    void* val;

    GET_ATTRIB(type, val);
    int type = *(int*)val;
    TRACE("left side is %s", (type) ? "list_reference" : "IDENTIFIER");

    if(type) {
        TRAVERSE_OBJ(list_reference);
    }
#ifdef TRACE_AST_STATE
    else {
        TRACE_STRING(IDENTIFIER);
    }
#endif

    RETURN;
}

static void assignment(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_ASSIGNMENT);

    TRAVERSE_OBJ(assignment_left);
    TRAVERSE_OBJ(assignment_right);

    RETURN;
}

static void raw_value(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_RAW_VALUE);

#ifdef TRACE_AST_STATE
    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    int type = *(int*)ptr;
    GET_ATTRIB(value, ptr);


    switch(type) {
        case IDENTIFIER:
            TRACE("identifier: %s", (const char*)ptr);
            break;
        case INTEGER_LIT:
            TRACE("integer literal: %ld", *(int64_t*)ptr);
            break;
        case FLOAT_LIT:
            TRACE("float literal: %lf", *(double*)ptr);
            break;
        default:
            FATAL("unknown node type in %d", type);
    }
#endif

    RETURN;
}

static void expr_primary(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXPR_PRIMARY);

    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    ast_type_t type = *(ast_type_t*)ptr;
    GET_ATTRIB(node, ptr);

    switch(type) {
        case AST_RAW_VALUE:
            raw_value(ptr, pre, post);
            break;
        case AST_FORMATTED_STRING:
            formatted_string(ptr, pre, post);
            break;
        case AST_LIST_REFERENCE:
            list_reference(ptr, pre, post);
            break;
        case AST_FUNC_REFERENCE:
            func_reference(ptr, pre, post);
            break;
        case AST_EXPRESSION:
            expression(ptr, pre, post);
            break;
        default:
            FATAL("unknown node type in %d", type);
    }

    RETURN;
}

static void list_reference(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LIST_REFERENCE);

    TRACE_STRING(IDENTIFIER);
    TRAVERSE_OBJ(list_ref_param_list);

    RETURN;
}

static void list_ref_value(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LIST_REF_VALUE);

    void* ptr = NULL;
    GET_ATTRIB(type, ptr);
    int type = *(int*)ptr;

    switch(type) {
        case 0:
            TRACE_STRING(IDENTIFIER);
            break;
        case 1:
            TRACE_INT(INTEGER_LIT);
            break;
        case 2:
            TRACE_STRING(STRING_LIT);
            break;
        case 3:
            TRAVERSE_OBJ(list_reference);
            break;
        case 4:
            TRAVERSE_OBJ(func_reference);
            break;
        case 5:
            TRAVERSE_OBJ(list_ref_param);
            break;
        default:
            FATAL("unknown node type in %d", type);
    }

    RETURN;
}

static void list_ref_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LIST_REF_PARAM);

    TRAVERSE_OBJ(list_ref_value);

    RETURN;
}

static void list_ref_param_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LIST_REF_PARAM_LIST);

    TRAVERSE_OBJ_LIST(list_ref_param);

    RETURN;
}

static void func_reference(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_FUNC_REFERENCE);

    TRAVERSE_OBJ(expression_list_param);
    TRACE_STRING(IDENTIFIER);

    RETURN;
}

static void expression(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXPRESSION);

    void* data;
    if(FIND_ATTRIB(expr_primary, data))
        expr_primary(data, pre, post);
    else {
        GET_ATTRIB(oper, data);
        if(*(int*)data == NOT_OPER || *(int*)data == UNARY_MINUS_OPER) {
            GET_ATTRIB(right, data);
            expression(data, pre, post);

            TRACE_TOKEN(oper);
        }
        else {
            GET_ATTRIB(left, data);
            expression(data, pre, post);
            GET_ATTRIB(right, data);
            expression(data, pre, post);

            TRACE_TOKEN(oper);
        }
    }

    RETURN;
}

static void expression_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXPRESSION_LIST);

    TRAVERSE_OBJ_LIST(expression);

    RETURN;
}

static void expression_list_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXPRESSION_LIST_PARAM);

    TRAVERSE_OPTION_OBJ(expression_list);

    RETURN;
}

static void expression_param(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_EXPRESSION_PARAM);

    TRAVERSE_OPTION_OBJ(expression);

    RETURN;
}

static void dict_init(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DICT_INIT);

    TRAVERSE_OBJ(dict_init_item_list);

    RETURN;
}

static void list_init(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_LIST_INIT);

    TRAVERSE_OBJ(expression_list);

    RETURN;
}

static void dict_init_item(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DICT_INIT_ITEM);

    TRACE_STRING(STRING_LIT);
    TRAVERSE_OBJ(expression);

    RETURN;
}

static void dict_init_item_list(ast_node_t* node, void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    ENTER(AST_DICT_INIT_ITEM_LIST);

    TRAVERSE_OBJ_LIST(dict_init_item);

    RETURN;
}

ast_node_t* create_ast_node(ast_type_t type) {

    ast_node_t* node = _ALLOC_DS(ast_node_t);
    node->type       = type;
    node->table      = create_hashtable();

    int val = get_line_no();
    insert_hashtable(node->table, "line_no", _COPY_DS(&val, int));
    val = get_col_no();
    insert_hashtable(node->table, "column_no", _COPY_DS(&val, int));
    insert_hashtable(node->table, "file_name", _COPY_STRING(get_file_name()));

    return node;
}

void add_ast_node_attrib(ast_node_t* node, const char* key, void* attrib) {

    insert_hashtable(node->table, key, attrib);
}

void add_ast_node_list_item(ast_node_t* node, const char* key, void* attrib) {

    void* lst;
    find_hashtable(node->table, key, &lst);
    if(lst == NULL) {
        lst = create_pointer_list();
        insert_hashtable(node->table, key, lst);
    }

    add_pointer_list(lst, attrib);
}

int get_ast_node_attrib(ast_node_t* node, const char* key, ast_node_t** val) {

    void* tmp;
    int retv = find_hashtable(node->table, key, &tmp);
    *val     = (ast_node_t*)(tmp); // hide the stupid cast
    return retv;
}

void traverse_ast(void (*pre)(ast_node_t*), void (*post)(ast_node_t*)) {

    START;

    ASSERT(root_node != NULL, "root node is NULL!");
    program(root_node, pre, post);

    FINISH;
}

const char* node_type_to_str(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "AST_PROGRAM" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "AST_PROGRAM_ITEM_LIST" :
            (type == AST_PROGRAM_ITEM)          ? "AST_PROGRAM_ITEM" :
            (type == AST_TYPE_NAME)             ? "AST_TYPE_NAME" :
            (type == AST_FORMATTED_STRING)      ? "AST_FORMATTED_STRING" :
            (type == AST_IMPORT_STATEMENT)      ? "AST_IMPORT_STATEMENT" :
            (type == AST_DATA_DECLARATION)      ? "AST_DATA_DECLARATION" :
            (type == AST_DATA_DECLARATION_LIST) ? "AST_DATA_DECLARATION_LIST" :
            (type == AST_DATA_DEFINITION)       ? "AST_DATA_DEFINITION" :
            (type == AST_FUNC_DEFINITION)       ? "AST_FUNC_DEFINITION" :
            (type == AST_FUNC_PARAMS)           ? "AST_FUNC_PARAMS" :
            (type == AST_FUNC_BODY)             ? "AST_FUNC_BODY" :
            (type == AST_FUNC_NAME)             ? "AST_FUNC_NAME" :
            (type == AST_FUNC_BODY_LIST)        ? "AST_FUNC_BODY_LIST" :
            (type == AST_LOOP_BODY)             ? "AST_LOOP_BODY" :
            (type == AST_LOOP_BODY_LIST)        ? "AST_LOOP_BODY_LIST" :
            (type == AST_LOOP_BODY_DIFFS)       ? "AST_LOOP_BODY_DIFFS" :
            (type == AST_LOOP_BODY_ELEM)        ? "AST_LOOP_BODY_ELEM" :
            (type == AST_FUNC_BODY_ELEM)        ? "AST_FUNC_BODY_ELEM" :
            (type == AST_TRACE_STATEMENT)       ? "AST_TRACE_STATEMENT" :
            (type == AST_PRINT_STATEMENT)       ? "AST_PRINT_STATEMENT" :
            (type == AST_EXIT_STATEMENT)        ? "AST_EXIT_STATEMENT" :
            (type == AST_RETURN_STATEMENT)      ? "AST_RETURN_STATEMENT" :
            (type == AST_TRYEXCEPT_STATEMENT)   ? "AST_TRYEXCEPT_STATEMENT" :
            (type == AST_TRY_CLAUSE)            ? "AST_TRY_CLAUSE" :
            (type == AST_EXCEPT_SEGMENT)        ? "AST_EXCEPT_SEGMENT" :
            (type == AST_EXCEPT_CLAUSE_LIST)    ? "AST_EXCEPT_CLAUSE_LIST" :
            (type == AST_FINAL_EXCEPT_CLAUSE)   ? "AST_FINAL_EXCEPT_CLAUSE" :
            (type == AST_EXCEPT_CLAUSE)         ? "AST_EXCEPT_CLAUSE" :
            (type == AST_RAISE_STATEMENT)       ? "AST_RAISE_STATEMENT" :
            (type == AST_IF_CLAUSE)             ? "AST_IF_CLAUSE" :
            (type == AST_IFELSE_STATEMENT)      ? "AST_IFELSE_STATEMENT" :
            (type == AST_ELSE_SEGMENT)          ? "AST_ELSE_SEGMENT" :
            (type == AST_FINAL_ELSE_SEGMENT)    ? "AST_FINAL_ELSE_SEGMENT" :
            (type == AST_ELSE_CLAUSE_LIST)      ? "AST_ELSE_CLAUSE_LIST" :
            (type == AST_ELSE_CLAUSE)           ? "AST_ELSE_CLAUSE" :
            (type == AST_WHILE_CLAUSE)          ? "AST_WHILE_CLAUSE" :
            (type == AST_WHILE_STATEMENT)       ? "AST_WHILE_STATEMENT" :
            (type == AST_DO_STATEMENT)          ? "AST_DO_STATEMENT" :
            (type == AST_ASSIGNMENT_RIGHT)      ? "AST_ASSIGNMENT_RIGHT" :
            (type == AST_ASSIGNMENT)            ? "AST_ASSIGNMENT" :
            (type == AST_RAW_VALUE)             ? "AST_RAW_VALUE" :
            (type == AST_EXPR_PRIMARY)          ? "AST_EXPR_PRIMARY" :
            (type == AST_LIST_REFERENCE)        ? "AST_LIST_REFERENCE" :
            (type == AST_LIST_REF_VALUE)        ? "AST_LIST_REF_VALUE" :
            (type == AST_LIST_REF_PARAM)        ? "AST_LIST_REF_PARAM" :
            (type == AST_LIST_REF_PARAM_LIST)   ? "AST_LIST_REF_PARAM_LIST" :
            (type == AST_FUNC_REFERENCE)        ? "AST_FUNC_REFERENCE" :
            (type == AST_EXPRESSION)            ? "AST_EXPRESSION" :
            (type == AST_EXPRESSION_LIST)       ? "AST_EXPRESSION_LIST" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "AST_EXPRESSION_LIST_PARAM" :
            (type == AST_EXPRESSION_PARAM)      ? "AST_EXPRESSION_PARAM" :
                                                  "UNKNOWN";
}

const char* node_type_to_name(ast_node_t* node) {

    ast_type_t type = node->type;

    return (type == AST_PROGRAM)                ? "program" :
            (type == AST_PROGRAM_ITEM_LIST)     ? "program_item_list" :
            (type == AST_PROGRAM_ITEM)          ? "program_item" :
            (type == AST_TYPE_NAME)             ? "type_name" :
            (type == AST_FORMATTED_STRING)      ? "formatted_string" :
            (type == AST_IMPORT_STATEMENT)      ? "import_statement" :
            (type == AST_DATA_DECLARATION)      ? "data_declaration" :
            (type == AST_DATA_DECLARATION_LIST) ? "data_declaration_list" :
            (type == AST_DATA_DEFINITION)       ? "data_definition" :
            (type == AST_FUNC_DEFINITION)       ? "func_definition" :
            (type == AST_FUNC_PARAMS)           ? "func_params" :
            (type == AST_FUNC_BODY)             ? "func_body" :
            (type == AST_FUNC_NAME)             ? "func_name" :
            (type == AST_FUNC_BODY_LIST)        ? "func_body_list" :
            (type == AST_LOOP_BODY)             ? "loop_body" :
            (type == AST_LOOP_BODY_LIST)        ? "loop_body_list" :
            (type == AST_LOOP_BODY_DIFFS)       ? "loop_body_diffs" :
            (type == AST_LOOP_BODY_ELEM)        ? "loop_body_elem" :
            (type == AST_FUNC_BODY_ELEM)        ? "func_body_elem" :
            (type == AST_TRACE_STATEMENT)       ? "trace_statement" :
            (type == AST_PRINT_STATEMENT)       ? "print_statement" :
            (type == AST_EXIT_STATEMENT)        ? "exit_statement" :
            (type == AST_RETURN_STATEMENT)      ? "return_statement" :
            (type == AST_TRYEXCEPT_STATEMENT)   ? "tryexcept_statement" :
            (type == AST_TRY_CLAUSE)            ? "try_clause" :
            (type == AST_EXCEPT_SEGMENT)        ? "except_segment" :
            (type == AST_EXCEPT_CLAUSE_LIST)    ? "except_clause_list" :
            (type == AST_FINAL_EXCEPT_CLAUSE)   ? "final_except_clause" :
            (type == AST_EXCEPT_CLAUSE)         ? "except_clause" :
            (type == AST_RAISE_STATEMENT)       ? "raise_statement" :
            (type == AST_IF_CLAUSE)             ? "if_clause" :
            (type == AST_IFELSE_STATEMENT)      ? "ifelse_statement" :
            (type == AST_ELSE_SEGMENT)          ? "else_segment" :
            (type == AST_FINAL_ELSE_SEGMENT)    ? "final_else_segment" :
            (type == AST_ELSE_CLAUSE_LIST)      ? "else_clause_list" :
            (type == AST_ELSE_CLAUSE)           ? "else_clause" :
            (type == AST_WHILE_CLAUSE)          ? "while_clause" :
            (type == AST_WHILE_STATEMENT)       ? "while_statement" :
            (type == AST_DO_STATEMENT)          ? "do_statement" :
            (type == AST_ASSIGNMENT_RIGHT)      ? "assignment_right" :
            (type == AST_ASSIGNMENT)            ? "assignment" :
            (type == AST_RAW_VALUE)             ? "raw_value" :
            (type == AST_EXPR_PRIMARY)          ? "expr_primary" :
            (type == AST_LIST_REFERENCE)        ? "list_reference" :
            (type == AST_LIST_REF_VALUE)        ? "list_ref_value" :
            (type == AST_LIST_REF_PARAM)        ? "list_ref_param" :
            (type == AST_LIST_REF_PARAM_LIST)   ? "list_ref_param_list" :
            (type == AST_FUNC_REFERENCE)        ? "func_reference" :
            (type == AST_EXPRESSION)            ? "expression" :
            (type == AST_EXPRESSION_LIST)       ? "expression_list" :
            (type == AST_EXPRESSION_LIST_PARAM) ? "expression_list_param" :
            (type == AST_EXPRESSION_PARAM)      ? "expression_param" :
                                                  "UNKNOWN";
}
