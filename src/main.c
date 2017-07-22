
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"

int main(int argc, char **argv) {

    init_errors(stdout);
    //open_file(argv[1]);
    open_file("../tests/test1.txt");
    phase1_parse_all();
    //yyparse();
    //printf("number of lines = %d\n", get_line_number());
    return 0;
}

/*
void yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    exit(1);
}
*/
