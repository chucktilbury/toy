
/*
 * Support routines for the parser symbol table creation.
 *
 */
#include <stdio.h>
#include <string.h>

#include "symtab.h"
#include "hash.h"
#include "memory.h"

#define MAX_BUFFER 1024

#define FIND_ATTRIB(node, name, ptr) find_hashtable(node->table, #name, &ptr)

#define GET_ATTRIB(node, name, ptr)                                  \
    do {                                                             \
        if(!FIND_ATTRIB(node, name, ptr))                            \
            FATAL("improper node format: \"%s\" not found", #name);  \
        else if(ptr == NULL)                                         \
            FATAL("improper node format: \"%s\" is invalid", #name); \
    } while(0)

#define TRACE_SYMTAB
#ifdef TRACE_SYMTAB
#include "trace.h"
#define TRACE(...)                                 \
    do {                                           \
        printf("%*sSYM TRACE: ", trace_depth, ""); \
        printf(__VA_ARGS__);                       \
        printf("\n");                              \
    } while(0)

#define ENTER                                      \
    do {                                           \
        printf("%*sSYM ENTER: ", trace_depth, ""); \
        printf("%s\n", __func__);                  \
        trace_depth += DEPTH_INC;                  \
    } while(0)

#define RETURN(...)                                 \
    do {                                            \
        trace_depth -= DEPTH_INC;                   \
        printf("%*sSYM RETURN: ", trace_depth, ""); \
        printf("%s\n", __func__);                   \
        return __VA_ARGS__;                         \
    } while(0)

#else
#define TRACE(...)
#define ENTER
#define RETURN(...)         \
    do {                    \
        return __VA_ARGS__; \
    } while(0)
#endif

static symbol_table_t* symbol_table   = NULL;
static pointer_list_t* symbol_context = NULL;
/*
static inline symbol_t* create_symbol(void) {

    // note that _ALLOC* clears the memory.
    return _ALLOC_DS(symbol_t);
}*/

static inline void add_symbol(symbol_t* sym) {

    insert_hashtable(symbol_table->table, sym->name, sym);
}

static inline void push_sym_context(symbol_table_t* tab) {

#ifdef TRACE_SYMTAB
    TRACE("PUSH SYMBOLIC CONTEXT");
    trace_depth += DEPTH_INC;
#endif

    push_pointer_list(symbol_context, tab);
}

static inline void* pop_sym_context(void) {

#ifdef TRACE_SYMTAB
    trace_depth -= DEPTH_INC;
    TRACE("POP SYMBOLIC CONTEXT");
#endif

    return pop_pointer_list(symbol_context);
}

static inline symbol_table_t* peek_sym_context(void) {

    return peek_pointer_list(symbol_context);
}

static inline symbol_table_t* create_symtab(void) {

    symbol_table_t* tab = _ALLOC_DS(symbol_table_t);
    tab->table          = create_hashtable();
    tab->parent         = peek_sym_context();

    return tab;
}

static inline void create_body(ast_node_t* node) {

    ENTER;
    RETURN();
}

static inline void create_data_symbol(ast_node_t* node) {

    ENTER;
    RETURN();
}

static inline void create_func_symbol(ast_node_t* node) {

    ENTER;
    RETURN();
}

static inline void create_exception_symbol(ast_node_t* node) {

    ENTER;
    RETURN();
}

static inline void create_except_id(ast_node_t* node) {

    ENTER;
    RETURN();
}

static inline void create_for_loop(ast_node_t* node) {

    ENTER;
    RETURN();
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY:
        case AST_LOOP_BODY:
            create_body(node);
            break;

        case AST_DATA_DEFINITION:
            create_data_symbol(node);
            break;

        case AST_FUNC_DEFINITION:
            create_func_symbol(node);
            break;

        case AST_FINAL_EXCEPT_CLAUSE:
        case AST_EXCEPT_SEGMENT:
            create_exception_symbol(node);
            break;

        case AST_EXCEPT_ID:
            create_except_id(node);
            break;

        case AST_FOR_STATEMENT:
            create_for_loop(node);
            break;

        default:
            break;
    }
}

static void post(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY:
        case AST_LOOP_BODY:
        case AST_FUNC_DEFINITION:
        case AST_FINAL_EXCEPT_CLAUSE:
        case AST_EXCEPT_SEGMENT:
        case AST_FOR_STATEMENT:
            pop_sym_context();
            break;

        default:
            break;
    }
}

symbol_t* create_symbol(symbol_class_t type, token_t* tok) {

    symbol_t* ptr = _ALLOC_DS(symbol_t);

    ptr->sym_class = type;
    ptr->tok       = tok;

    return ptr;
}
