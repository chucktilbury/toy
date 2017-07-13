
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "parse.h"

int main(int argc, char **argv) {

    open_file(argv[1]);
    yyparse();
    //printf("number of lines = %d\n", get_line_number());
    return 0;
}

/*
void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}
*/
