/*
 * This module traverses the AST and verifies that all symbols referenced are
 * present and that they have acceptable type references. If there is a problem
 * then publish a syntax error. When the emit passes are called, then the
 * existance and use of variables has already been verified.
 */
#include <stdio.h>

#include "symtab.h"
#include "ast.h"
#include "errors.h"
#include "parser.h"

static sym_context_t* context = NULL;

// defined in parser.y
const char* tokenToStr(int tok);

#define FIND_ATTRIB(node, name, ptr) find_hashtable(node->table, #name, &ptr)

#define GET_ATTRIB(node, name, ptr)                                  \
    do {                                                             \
        if(!FIND_ATTRIB(node, name, ptr))                            \
            FATAL("improper node format: \"%s\" not found", #name);  \
        else if(ptr == NULL)                                         \
            FATAL("improper node format: \"%s\" is invalid", #name); \
    } while(0)

#define DATA_NOT_DEFINED(node)                                                             \
    do {                                                                                   \
        void* str;                                                                         \
        GET_ATTRIB(node, IDENTIFIER, str);                                                 \
        node_syntax_error(node, "cannot find definition for symbol %s", (const char*)str); \
        return;                                                                            \
    } while(0)

#define TRACE_SYM_REF
#ifdef TRACE_SYM_REF
static int depth           = 0;
static const int depth_inc = 4;
#define TRACE(...)                       \
    do {                                 \
        printf("%*sTRACE: ", depth, ""); \
        printf(__VA_ARGS__);             \
        printf("\n");                    \
    } while(0)

#define ENTER                            \
    do {                                 \
        printf("%*sENTER: ", depth, ""); \
        printf("%s\n", __func__);        \
        depth += depth_inc;              \
    } while(0)

#define RETURN                            \
    do {                                  \
        depth -= depth_inc;               \
        printf("%*sRETURN: ", depth, ""); \
        printf("%s\n", __func__);         \
    } while(0)

#else
#define TRACE(...)
#define ENTER
#define RETURN return
#endif

static inline void push_context(void) {

    TRACE("PUSH SYMBOLIC CONTEXT");
#ifdef TRACE_SYM_REF
    depth += depth_inc;
#endif

    context = context->child;
    ASSERT(context != NULL, "symbol context stack overrun");
}

static inline void pop_context(void) {

#ifdef TRACE_SYM_REF
    depth -= depth_inc;
#endif
    TRACE("POP SYMBOLIC CONTEXT");

    context = context->parent;
    ASSERT(context != NULL, "symbol context stack underrun");
}

static inline symbol_t* find_symbol(const char* name) {

    void* ptr;
    symbol_t* sym       = NULL;
    sym_context_t* tctx = context;
    TRACE("find symbol: %s", name);

    while(true) {
        if(find_hashtable(tctx->table, name, &ptr)) {
            sym = (symbol_t*)ptr;
            TRACE("..... found");
            break;
        }
        else {
            tctx = tctx->parent;
            if(tctx == NULL) {
                TRACE("..... not found");
                break;
            }
        }
    }

    // return NULL if the symbol was not found
    return sym;
}

static inline void func_reference(ast_node_t* node) {

    // function reference has to have the name built from the parameters
    ENTER;

    // read the reference name.

    // read the parameter names and verify the types

    // build the actual decorated function name

    // verify the function exists.

    RETURN;
}

static inline void list_ref_value(ast_node_t* node) {

    ENTER;

    void* ptr;
    GET_ATTRIB(node, type, ptr);
    int type = *(int*)ptr;

    if(type == 0) {
        GET_ATTRIB(node, IDENTIFIER, ptr);
        const char* str = (const char*)ptr;
        symbol_t* sym   = find_symbol(str);
        if(sym == NULL) {
            node_syntax_error(node, "cannot find definition for symbol \"%s\"", (const char*)str);
        }
        else {
            TRACE("defined as %s", tokenToStr(sym->data_type));
            // acceptable value types to index an array or a hash
            switch(sym->data_type) {
                case LIST:
                case HASH:
                case STRING_LIT:
                case INTEGER_LIT:
                case INTEGER:
                case STRING:
                    break;
                default:
                    node_syntax_error(node, "invalid list or hash index: %s", tokenToStr(sym->data_type));
            }
        }
    }

    RETURN;
}

static inline void list_reference(ast_node_t* node) {

    ENTER;

    void* ptr;

    // must be data declared as a LIST or a DICT
    GET_ATTRIB(node, IDENTIFIER, ptr);
    const char* str = (const char*)ptr;
    symbol_t* sym   = find_symbol(str);
    if(sym == NULL) {
        node_syntax_error(node, "cannot find definition for symbol \"%s\"", (const char*)str);
    }
    else {
        TRACE("defined as %s", tokenToStr(sym->data_type));
        if(sym->data_type != LIST && sym->data_type != HASH) {
            node_syntax_error(node, "expected a LIST or a DICT but got a %s", tokenToStr(sym->data_type));
        }
    }

    RETURN;
}

static inline void raw_value(ast_node_t* node) {

    ENTER;
    void* ptr;

    GET_ATTRIB(node, type, ptr);
    int type = *(int*)ptr;

    if(type == IDENTIFIER) {
        // must be data declared as a LIST or a DICT
        GET_ATTRIB(node, value, ptr);
        const char* str = (const char*)ptr;
        symbol_t* sym   = find_symbol(str);
        if(sym != NULL)
            TRACE("defined as %s", tokenToStr(sym->data_type));
    }
    // else nothing to do

    RETURN;
}

static inline void assignment_left(ast_node_t* node) {

    ENTER;

    void* ptr;
    GET_ATTRIB(node, type, ptr);
    int type = *(int*)ptr;

    if(type == 0) {
        GET_ATTRIB(node, IDENTIFIER, ptr);
        const char* str = (const char*)ptr;
        symbol_t* sym   = find_symbol(str);
        if(sym == NULL) {
            node_syntax_error(node, "cannot find definition for symbol \"%s\"", (const char*)str);
        }
#ifdef TRACE_SYM_REF
        else
            TRACE("defined as %s", tokenToStr(sym->data_type));
#endif
    }

    RETURN;
}

static inline void except_segment(ast_node_t* node) {

    ENTER;


    void* ptr;
    GET_ATTRIB(node, ename, ptr);
    const char* name = (const char*)ptr;

    sym_context_t* root = get_symtab();
    if(find_hashtable(root->table, name, &ptr)) {
        symbol_t* sym = (symbol_t*)ptr;
    }
    else
        node_syntax_error(node, "exception named \"%s\" is not defined", name);

    RETURN;
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_REFERENCE:
            func_reference(node);
            break;

        case AST_LIST_REF_VALUE:
            list_ref_value(node);
            break;

        case AST_LIST_REFERENCE:
            list_reference(node);
            break;

        case AST_RAW_VALUE:
            raw_value(node);
            break;

        case AST_ASSIGNMENT_LEFT:
            assignment_left(node);
            break;

        case AST_EXCEPT_SEGMENT:
            except_segment(node);
            break;


        case AST_FUNC_BODY:
        case AST_LOOP_BODY:
        case AST_FUNC_DEFINITION:
            push_context();
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
            pop_context();
            break;

        default:
            break;
    }
}

void check_sym_refs(void) {

    ENTER;

    if(!get_errors()) {

        context = get_symtab();
        traverse_ast(pre, post);
    }

    RETURN;
}
