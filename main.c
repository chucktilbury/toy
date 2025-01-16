
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

// #include "errors.h"
// #include "pointer_list.h"
#include "parser.h"
#include "scanner.h"

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "syntax: %s filename\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if(fp == NULL) {
        fprintf(stderr, "error: %s cannot open input file: %s: %s", argv[0], argv[1], strerror(errno));
        return 1;
    }

    yyset_in(fp);
    yyparse();


    return 0;
}
