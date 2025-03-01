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

// defined in parser.y
const char* token_to_str(int);

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

    EMIT("//------------------------\n");
    EMIT("//---- generated code ----\n");
    EMIT("//----- DO NOT EDIT ------\n");
    EMIT("//------------------------\n");
    EMIT("// source: %s\n", get_base_file_name());
    EMIT("// date:   %s", ctime(&t));
    EMIT("// user:   %s\n", getenv("USER"));
    EMIT("// dir:    %s\n\n", getcwd(NULL, 0));

    EMIT("#include <stdio.h>\n");
    EMIT("#include <stdlib.h>\n");
    EMIT("#include <stdint.h>\n");
    EMIT("#include <stdbool.h>\n");
    EMIT("#include <string.h>\n\n");
    EMIT("#include \"runtime.h\"\n\n");

    EMIT("// intrinsic types for TOY code\n")
    EMIT("typedef double TOY_FLOAT;\n");
    EMIT("typedef int64_t TOY_INTEGER;\n");
    EMIT("typedef void TOY_NOTHING;\n");
    EMIT("typedef bool TOY_BOOL;\n");
    EMIT("typedef string_buffer_t* TOY_STRING;\n\n");

    EMIT("//--- begin user code ---\n\n");
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
    EMIT("\n//---------------------------\n");
    EMIT("//-- end of generated code --\n");
    EMIT("//---------------------------\n\n");

    RETURN();
}

/**
 * @brief Emit the code for a data definition.
 * 
 * @param node 
 */
static inline void emit_data_definition(ast_data_definition_t* node) {

    ENTER;

    ast_data_declaration_t* dd = node->data_declaration;

    if(dd->is_const)
        EMIT("const ");

    EMIT("TOY_%s TOY_%s ", token_to_str(dd->type), dd->IDENTIFIER->raw);
    dd->node.emit = false;

    EMIT("= expr;\n");

    RETURN();
}

/**
 * @brief Emit the code for a data declaration.
 * 
 * @param node 
 */
static inline void emit_data_declaration(ast_data_declaration_t* node) {

    ENTER;

    if(node->node.emit) {
        if(node->is_const)
            EMIT("const ");

        EMIT("TOY_%s TOY_%s;\n", token_to_str(node->type), node->IDENTIFIER->raw);
    }

    RETURN();
}

/**
 * @brief Emit the code for a function definition.
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

static inline void emit_pre_start_block(ast_start_block_t* node) {

    ENTER;

    EMIT("int main(int argc, char** argv, char** arge) {\n");
    EMIT("runtime_main_init(argc, argv, arge);\n");

    RETURN();
}

static inline void emit_post_start_block(ast_start_block_t* node) {

    ENTER;

    EMIT("return TOY_RETV;\n");
    EMIT("}\n\n");

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

        case AST_DATA_DECLARATION:
            emit_data_declaration((ast_data_declaration_t*)node);
            break;

        case AST_FUNC_DEFINITION:
            emit_func_definition((ast_func_definition_t*)node);
            break;

        case AST_START_BLOCK:
            emit_pre_start_block((ast_start_block_t*)node);
            break;

        case AST_FUNC_BODY:
            emit_func_body((ast_func_body_t*)node);
            break;

        case AST_LOOP_BODY:
            emit_loop_body((ast_loop_body_t*)node);
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

        case AST_START_BLOCK:
            emit_post_start_block((ast_start_block_t*)node);
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
