
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

#define MAX_BUFFER 1024

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
#include "trace.h"
#define TRACE(...)                       \
    do {                                 \
        printf("%*sSYM TRACE: ", trace_depth, ""); \
        printf(__VA_ARGS__);             \
        printf("\n");                    \
    } while(0)

#define ENTER                            \
    do {                                 \
        printf("%*sSYM ENTER: ", trace_depth, ""); \
        printf("%s\n", __func__);        \
        trace_depth += DEPTH_INC;              \
    } while(0)

#define RETURN(...)                       \
    do {                                  \
        trace_depth -= DEPTH_INC;               \
        printf("%*sSYM RETURN: ", trace_depth, ""); \
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
        token_syntax_error(sym->tok, "attempt to define duplicate name in context: %s", sym->name);
}

static inline void push_sym_context(void) {

    TRACE("PUSH SYMBOLIC CONTEXT");
#ifdef TRACE_SYMTAB
    trace_depth += DEPTH_INC;
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
    trace_depth -= DEPTH_INC;
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
    char buffer[MAX_BUFFER];

    // functions are always in the root context.
    // node is a function_definition
    symbol_t* sym = create_symbol();

    // function name and type
    GET_ATTRIB(node, func_name, ptr);
    ast_node_t* fname = (ast_node_t*)ptr;
    GET_ATTRIB(fname, token, ptr);
    sym->tok = (token_t*)ptr;
    GET_ATTRIB(fname, is_iter, ptr);
    sym->is_iter = *(int*)ptr;

    // get the type name
    GET_ATTRIB(fname, type_name, ptr);
    ast_node_t* tmp = (ast_node_t*)ptr;
    GET_ATTRIB(tmp, token, ptr);
    sym->data_type = ((token_t*)ptr)->type;
    snprintf(buffer, sizeof(buffer), "FUNC_%s_", sym->tok->raw);

    // do the function parameters and decorate the name.
    GET_ATTRIB(node, func_params, ptr);
    tmp = (ast_node_t*)ptr;
    GET_ATTRIB(tmp, data_declaration_list, ptr);
    tmp = (ast_node_t*)ptr;
    GET_ATTRIB(tmp, list, ptr);
    // list of data declarations
    pointer_list_t* list = (pointer_list_t*)ptr;
    int mark             = 0;

    // push the context for the parameters
    push_sym_context();
    while(NULL != (tmp = iterate_pointer_list(list, &mark))) {
        symbol_t* psym = create_symbol();
        char buf[MAX_BUFFER];

        GET_ATTRIB(tmp, token, ptr);
        psym->tok         = (token_t*)ptr;
        psym->is_const    = false; // const params are illegal
        psym->is_init     = true;  // init by the caller
        psym->symbol_type = SYM_DATA;
        snprintf(buf, sizeof(buf), "DATA_%s", psym->tok->raw);
        psym->name = _COPY_STRING(buf);

        GET_ATTRIB(tmp, type_name, ptr);
        tmp = (ast_node_t*)ptr;
        GET_ATTRIB(tmp, token, ptr);
        token_t* tok    = (token_t*)ptr;
        psym->data_type = tok->type;

        size_t len = strlen(buffer);
        snprintf(&buffer[len], MAX_BUFFER - len - 1, "_%s", tokenToStr(tok->type));

        add_symbol(psym);
    }

    sym->name = _COPY_STRING(buffer);

    // add the function to the root context
    if(!insert_hashtable(symtab->table, sym->name, sym))
        token_syntax_error(sym->tok, "attempt to define duplicate function name: %s", sym->name);

    RETURN();
}

static inline void create_data_symbol(ast_node_t* node) {

    ENTER;

    void* ptr;
    char buffer[MAX_BUFFER];

    symbol_t* sym    = create_symbol();
    sym->symbol_type = SYM_DATA;

    // data definition
    GET_ATTRIB(node, is_init, ptr);
    TRACE("is_init = %s", (*(int*)ptr) ? "TRUE" : "FALSE");
    sym->is_init = *(int*)ptr;
    GET_ATTRIB(node, data_declaration, ptr);
    ast_node_t* decl = (ast_node_t*)ptr;

    // data declaration
    GET_ATTRIB(decl, is_const, ptr);
    sym->is_const = *(int*)ptr;
    GET_ATTRIB(decl, token, ptr);
    sym->tok = (token_t*)ptr;
    snprintf(buffer, sizeof(buffer), "IDENT_%s", sym->tok->raw);
    sym->name = _COPY_STRING(buffer);
    GET_ATTRIB(decl, type_name, ptr);
    ast_node_t* type = (ast_node_t*)ptr;

    // data type
    GET_ATTRIB(type, token, ptr);
    token_t* tok = (token_t*)ptr;
    TRACE("type = %s", tokenToStr(tok->type));
    sym->data_type = tok->type;

    add_symbol(sym);

    RETURN();
}

static inline void create_exception_symbol(ast_node_t* node) {

    ENTER;

    void* ptr;
    char buffer[MAX_BUFFER];

    GET_ATTRIB(node, mname, ptr);
    token_t* tok = (token_t*)ptr;

    push_sym_context();
    symbol_t* sym    = create_symbol();
    sym->tok         = tok;
    sym->symbol_type = SYM_DATA;
    sym->data_type   = IDENTIFIER;
    sym->is_const    = true;
    sym->is_init     = false; // init at runtime by raise()
    snprintf(buffer, sizeof(buffer), "IDENT_%s", tok->raw);
    sym->name = _COPY_STRING(buffer);

    add_symbol(sym);

    RETURN();
}

static inline void create_except_id(ast_node_t* node) {

    ENTER;

    void* ptr;
    char buffer[MAX_BUFFER];

    GET_ATTRIB(node, token, ptr);
    token_t* tok = (token_t*)ptr;

    symbol_t* sym    = create_symbol();
    sym->tok         = tok;
    sym->symbol_type = SYM_EXCEPT;
    sym->data_type   = INTEGER;
    sym->is_const    = true;
    sym->is_init     = true; // initialized by the emit pass
    snprintf(buffer, sizeof(buffer), "EXCEPT_ID_%s", tok->raw);
    sym->name = _COPY_STRING(buffer);

    // add it to the root context
    if(!insert_hashtable(symtab->table, sym->name, sym))
        token_syntax_error(sym->tok, "attempt to define duplicate exception name: %s", sym->name);

    RETURN();
}

static inline void create_for_loop(ast_node_t* node) {

    ENTER;

    // If the type name is not present, then it's a reference, not a definition.
    if(hash_name_exists(node->table, "type_name")) {
        void* ptr;
        char buffer[MAX_BUFFER];

        push_sym_context();
        symbol_t* sym = create_symbol();

        GET_ATTRIB(node, token, ptr);
        sym->tok         = (token_t*)ptr;
        sym->symbol_type = SYM_DATA;
        sym->is_const    = false;
        sym->is_init     = false;
        snprintf(buffer, sizeof(buffer), "IDENT_%s", sym->tok->raw);
        sym->name = _COPY_STRING(buffer);


        GET_ATTRIB(node, type_name, ptr);
        ast_node_t* type = (ast_node_t*)ptr;
        GET_ATTRIB(type, token, ptr);
        sym->data_type = ((token_t*)ptr)->type;

        add_symbol(sym);
    }

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

        case AST_FOR_STATEMENT:
            TRACE("end of for statement");
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

        traverse_ast(pre, post);
        // traverse_ast(NULL, NULL);

        pop_sym_context();
    }

    RETURN(symtab);
}

sym_context_t* get_symtab(void) {

    return symtab;
}


static void dump_table(hash_table_t* tab) {

    ASSERT(tab != NULL, "");

    printf("%*scap = %d\n", trace_depth, "", tab->cap);
    printf("%*scount = %d\n", trace_depth, "", tab->count);
    for(int i = 0; i < tab->cap; i++) {
        if(tab->table[i] != NULL) {
            if(tab->table[i]->key != NULL) {
                symbol_t* sym = tab->table[i]->data;
                printf("%*s%s %s\n", trace_depth, "", tokenToStr(sym->data_type), sym->name);
            }
        }
    }
}

void dump_symtab(sym_context_t* tmp) {

    if(tmp != NULL) {
        trace_depth += DEPTH_INC;
        dump_symtab(tmp->child);
        trace_depth -= DEPTH_INC;
    }
}

void dump_symbols(void) {

    dump_symtab(symtab);
}
