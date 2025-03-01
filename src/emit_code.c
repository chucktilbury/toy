/**
 * @file emit_code.c
 * 
 * @brief Top level code emitter. This AST pass dispatches all other 
 * code emitters to the output file.
 * 
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ast.h"
#include "string_buffer.h"
#include "errors.h"
#include "fileio.h"

#define TRACE_EMIT

#ifdef TRACE_EMIT
#define USE_TRACE
#endif
#include "trace.h"


#define EMIT(...) append_string_buffer_fmt(buffer, __VA_ARGS__);

string_buffer_t* buffer;

/**
 * @brief Emit the code for the program start up.
 * 
 * @param node 
 */
static inline void emit_program_start(ast_program_t* node) {

    ENTER;

    (void)node;
    time_t t = time(NULL);

    EMIT("\n// generated code -- do not edit --\n");
    EMIT("// source: %s\n", get_base_file_name());
    EMIT("// date:   %s", ctime(&t));
    EMIT("// user:   %s\n", getenv("USER"));
    EMIT("// dir:    %s\n\n", getcwd(NULL, 0));

    RETURN();
}

/**
 * @brief Emit the code for the program end.
 * 
 * @param node 
 */
static inline void emit_program_end(ast_program_t* node) {

    ENTER;

    (void)node;
    EMIT("\n// end of generated code\n");

    RETURN();
}

/**
 * @brief Emite the code for a data definition.
 * 
 * @param node 
 */
static inline void emit_data_definition(ast_data_definition_t* node) {

    ENTER;


    RETURN();
}

/**
 * @brief Emite the code for a function definition.
 * 
 * @param node 
 */
static inline void emit_func_definition(ast_func_definition_t* node) {

    ENTER;


    RETURN();
}

/**
 * @brief Emit the code for a function body.
 * 
 * @param node 
 */
static inline void emit_func_body(ast_func_body_t* node) {

    ENTER;


    RETURN();
}

/**
 * @brief Emit the code for a loop body.
 * 
 * @param node 
 */
static inline void emit_loop_body(ast_loop_body_t* node) {

    ENTER;


    RETURN();
}

/**
 * @brief Callback run before the node is traversed.
 * 
 * @param node 
 */
static void pre(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_PROGRAM:
            emit_program_start((ast_program_t*)node);
            break;

        case AST_DATA_DEFINITION:
            emit_data_definition((ast_data_definition_t*)node);
            break;

        case AST_FUNC_DEFINITION:
            emit_func_definition((ast_func_definition_t*)node);
            break;

        case AST_FUNC_BODY:
            emit_func_body((ast_func_body_t*)node);
            break;

        case AST_LOOP_BODY:
            emit_loop_body((ast_loop_body_t*)node);
            break;

        case AST_START_BLOCK:
            break;

        default:
            break;
    }
}

/**
 * @brief Callback run after the node is traversed.
 * 
 * @param node 
 */
static void post(ast_node_t* node) {

    ast_type_t type = node->type;

    switch(type) {
        case AST_PROGRAM:
            emit_program_end((ast_program_t*)node);
            break;

        default:
            break;
    }
}

/**
 * @brief Top level entry point of the AST pass.
 * 
 * @param fname 
 */
void emit_code(const char* fname) {

    SEPARATOR;
    ENTER;

    buffer = create_string_buffer(NULL);

    if(!get_errors()) {
        traverse_ast(pre, post);
    }

    write_string_buffer(buffer, fname);

    RETURN();
}
