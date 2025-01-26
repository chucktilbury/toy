
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "errors.h"
#include "fileio.h"

// #include "errors.h"
// #include "pointer_list.h"
#include "ast.h"
#include "parser.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "syntax: %s filename\n", argv[0]);
        return 1;
    }

    open_file(argv[1]);

    yyparse();

    if(!get_errors())
        traverse_ast(NULL, NULL);

    return 0;
}
