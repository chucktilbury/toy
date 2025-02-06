#include <stdio.h>

#include "parser.h"
#include "fileio.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "%s: filename\n", argv[0]);
        return 1;
    }

    open_file(argv[1]);
    yyparse();
    return 0;
}
