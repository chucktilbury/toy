
/*
 * This AST pass creates the symbol table. All productions that do not
 * participate are deleted.
 *
 */
#include <stdio.h>
#include <string.h>

#include "symtab.h"
#include "hash.h"
#include "memory.h"
#include "ast.h"
#include "errors.h"
#include "pointer_list.h"
#include "parser.h"

const char* tokenToStr(int);

static sym_context_t* symtab  = NULL;
static sym_context_t* context = NULL;

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

#define RETURN(...)                       \
    do {                                  \
        depth -= depth_inc;               \
        printf("%*sRETURN: ", depth, ""); \
        printf("%s\n", __func__);         \
        return __VA_ARGS__;               \
    } while(0)

#else
#define TRACE(...)
#define ENTER
#define RETURN(...)         \
    do {                    \
        return __VA_ARGS__; \
    } while(0)
#endif

static inline symbol_t* create_symbol(void) {

    // note that _ALLOC* clears the memory.
    return _ALLOC_DS(symbol_t);
}

static inline void add_symbol(symbol_t* sym) {

    if(!insert_hashtable(context->table, sym->name, sym))
        node_syntax_error(sym->val.node, "attempt to define duplicate name in context: %s", sym->name);
}

static inline void push_sym_context(void) {

    TRACE("PUSH SYMBOLIC CONTEXT");
#ifdef TRACE_SYMTAB
    depth += depth_inc;
#endif

    sym_context_t* ptr;

    ptr        = _ALLOC_DS(sym_context_t);
    ptr->table = create_hashtable();

    if(context != NULL)
        context->child = ptr;
    ptr->parent = context;
    context     = ptr;
}

static inline sym_context_t* pop_sym_context(void) {

#ifdef TRACE_SYMTAB
    depth -= depth_inc;
#endif
    TRACE("POP SYMBOLIC CONTEXT");

    ASSERT(context != NULL, "symbolic context stack underrun");
    context = context->parent;

    return context;
}

static inline sym_context_t* peek_sym_context(void) {

    ENTER;

    if(context == NULL)
        context = symtab;

    RETURN(context);
}

static inline void create_func_symbol(ast_node_t* node) {

    ENTER;

    void* ptr;

    TRACE("function name is in the current symbolic context");
    // Get the function name declaration
    GET_ATTRIB(node, func_name, ptr);
    ast_node_t* func = (ast_node_t*)ptr;
    GET_ATTRIB(func, IDENTIFIER, ptr);
    // used later for dechor
    const char* func_name = _COPY_STRING((const char*)ptr);
    TRACE("name = %s", func_name);

    GET_ATTRIB(func, type_name, ptr);
    ast_node_t* type_name = (ast_node_t*)ptr;
    GET_ATTRIB(type_name, type, ptr);
    TRACE("type = %s", tokenToStr(*(int*)ptr));

    symbol_t* sym    = create_symbol();
    sym->val.node    = node;
    sym->symbol_type = SYM_FUNC;
    sym->data_type   = *(int*)ptr;
    sym->is_const    = true;
    sym->is_init     = true;
    // save the original name. the decorated name is in sym->name below
    sym->val.text              = func_name;
    sym_context_t* old_context = context;

    GET_ATTRIB(node, func_params, ptr);
    ast_node_t* func_params = (ast_node_t*)ptr;
    GET_ATTRIB(func_params, data_declaration_list, ptr);
    ast_node_t* data_declaration_list = (ast_node_t*)ptr;

    // get the parameter list
    GET_ATTRIB(data_declaration_list, list, ptr);
    pointer_list_t* lst = (pointer_list_t*)ptr;

    char dec_buf[1024];
    memset(dec_buf, 0, sizeof(dec_buf));
    strcat(dec_buf, func_name);
    strcat(dec_buf, "_");

    // Add the function name to the context and create a new one
    // for the parameters.
    TRACE("symbolic context for the function parameters");
    push_sym_context();

    int mark = 0;
    ast_node_t* data_declaration;
    while(NULL != (data_declaration = iterate_pointer_list(lst, &mark))) {

        GET_ATTRIB(data_declaration, is_const, ptr);
        TRACE("is_const = %s", (*(int*)ptr) ? "TRUE" : "FALSE");

        GET_ATTRIB(data_declaration, IDENTIFIER, ptr);
        const char* pname = _COPY_STRING((const char*)ptr);
        TRACE("name = %s", pname);

        GET_ATTRIB(data_declaration, type_name, ptr);
        ast_node_t* type = (ast_node_t*)ptr;

        GET_ATTRIB(type, type, ptr);
        TRACE("type = %s", tokenToStr(*(int*)ptr));

        char* tpt = (char*)tokenToStr(*(int*)ptr);
        // simply truncate strings that are too long
        strncat(dec_buf, "_", sizeof(dec_buf) - strlen(dec_buf) - 3);
        strncat(dec_buf, tpt, sizeof(dec_buf) - strlen(dec_buf) - strlen(tpt) - 2);

        symbol_t* dsym    = create_symbol();
        dsym->node        = data_declaration;
        dsym->symbol_type = SYM_DATA;
        dsym->data_type   = *(int*)ptr;
        dsym->is_const    = false;
        dsym->is_init     = false;
        // save the original name. the decorate name is in var.text
        dsym->name = pname;

        add_symbol(dsym);
    }

    strncat(dec_buf, "_FUNC", sizeof(dec_buf) - strlen(dec_buf) - 7);
    sym->name = _COPY_STRING(dec_buf);
    TRACE("decorated function name: %s", dec_buf);
    // now that we know the name ...
    if(!insert_hashtable(old_context->table, sym->name, sym))
        node_syntax_error(sym->node, "attempt to define duplicate name in context: %s", sym->name);

    RETURN();
}

static inline void create_data_symbol(ast_node_t* node) {

    ENTER;

    void* ptr;

    symbol_t* sym    = create_symbol();
    sym->node        = node;
    sym->symbol_type = SYM_DATA;

    // Get the data declaration and all of its attributes.
    GET_ATTRIB(node, type, ptr);
    TRACE("production type = %d", *(int*)ptr);

    GET_ATTRIB(node, is_init, ptr);
    TRACE("is_init = %s", (*(int*)ptr) ? "TRUE" : "FALSE");
    sym->is_init = *(int*)ptr;

    GET_ATTRIB(node, data_declaration, ptr);
    ast_node_t* decl = (ast_node_t*)ptr;

    GET_ATTRIB(decl, is_const, ptr);
    TRACE("is_const = %s", (*(int*)ptr) ? "TRUE" : "FALSE");
    sym->is_const = *(int*)ptr;

    GET_ATTRIB(decl, IDENTIFIER, ptr);
    TRACE("name = %s", (const char*)ptr);
    sym->name = _COPY_STRING(ptr);

    GET_ATTRIB(decl, type_name, ptr);
    ast_node_t* type = (ast_node_t*)ptr;

    GET_ATTRIB(type, type, ptr);
    TRACE("type = %s", tokenToStr(*(int*)ptr));
    sym->data_type = *(int*)ptr;

    add_symbol(sym);

    RETURN();
}

static inline void create_exception_symbol(ast_node_t* node) {

    ENTER;

    void* ptr;
    GET_ATTRIB(node, mname, ptr);
    const char* name = (const char*)ptr;

    push_sym_context();
    symbol_t* sym    = create_symbol();
    sym->node        = node;
    sym->symbol_type = SYM_DATA;
    sym->data_type   = STRING_LIT;
    sym->is_const    = true;
    sym->is_init     = true;
    sym->name        = _COPY_STRING(name);

    add_symbol(sym);

    RETURN();
}

static inline void create_except_id(ast_node_t* node) {

    ENTER;

    void* ptr;
    GET_ATTRIB(node, IDENTIFIER, ptr);
    const char* name = (const char*)ptr;

    symbol_t* sym    = create_symbol();
    sym->node        = node;
    sym->symbol_type = SYM_EXCEPT;
    sym->data_type   = INTEGER;
    sym->is_const    = true;
    sym->is_init     = true;
    sym->name        = _COPY_STRING(name);

    // add it to the root context
    if(!insert_hashtable(symtab->table, sym->name, sym))
        node_syntax_error(sym->node, "attempt to define duplicate exception name: %s", sym->name);

    RETURN();
}

static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_FUNC_BODY:
        case AST_LOOP_BODY:
            TRACE("symbolic context for function or loop body");
            push_sym_context();
            break;

        case AST_DATA_DEFINITION:
            TRACE("data definition");
            create_data_symbol(node);
            break;

        case AST_FUNC_DEFINITION:
            TRACE("function definition");
            create_func_symbol(node);
            break;

        case AST_FINAL_EXCEPT_CLAUSE:
        case AST_EXCEPT_SEGMENT:
            TRACE("except segment");
            create_exception_symbol(node);
            break;

        case AST_EXCEPT_ID:
            TRACE("create exception ID");
            create_except_id(node);
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
            TRACE("end of loop or function body definition");
            pop_sym_context();
            break;

        case AST_FUNC_DEFINITION:
            TRACE("end of function definition");
            pop_sym_context();
            break;

        case AST_FINAL_EXCEPT_CLAUSE:
        case AST_EXCEPT_SEGMENT:
            TRACE("end of exception segment");
            pop_sym_context();
            break;

        default:
            break;
    }
}

sym_context_t* create_symtab(void) {

    ENTER;

    if(!get_errors()) {
        push_sym_context();          // create the root context
        symtab = peek_sym_context(); // init the global pointer to it

        //traverse_ast(pre, post);
        traverse_ast(NULL, NULL);

        pop_sym_context();
    }

    RETURN(symtab);
}

sym_context_t* get_symtab(void) {

    return symtab;
}
