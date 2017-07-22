
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"

int run_parser(char *fname)
{
    int retv;
    ENTER();

    open_file(fname);
    retv = phase1_parse_all();

    DEBUG(9, "total number of lines = %d", get_total_lines());


    if(retv)
        return PHASE1_FAILED;
    else
        return PHASE1_SUCCESS;
}

int main(int argc, char **argv)
{

    init_errors(stdout);
    atexit(show_result);

    return run_parser("../tests/test1.txt");
}

