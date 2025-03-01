
#include <stdio.h>
// #include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
// #include <string.h>
// #include <errno.h>
// #include "errors.h"
#include "emit_code.h"
#include "fileio.h"

#include "errors.h"
// #include "pointer_list.h"
// #include "ast.h"
#include "func_reference.h"
#include "parser.h"
#include "symbol_table.h"
#include "sym_reference.h"
#include "dump_symbols.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "syntax: %s filename\n", argv[0]);
        return 1;
    }

    open_file(argv[1]);

    // run the parser
    yyparse();

    // traverse_ast(NULL, NULL);
    //  do the check passes
    create_symbol_table();
    check_sym_refs();
    create_func_references();
    emit_code("outfile.c");
    dump_symbols();

    printf("\ncompile %s: errors = %d warnings = %d\n\n",
           get_errors() ? "FAILED" : "SUCCEDED", get_errors(), get_warnings());

    return 0;
}
