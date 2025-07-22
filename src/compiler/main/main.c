
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cmdline.h"
#include "trace.h"
#include "errors.h"
#include "parser.h"

#include "tokens.h"
#include "file_io.h"

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("toy", "Toy is a small C-like language compiler", "0.1");
    add_cmdline('v', "verbosity", "verbosity", "From 0 to 10. Print more information", "0", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('p', "path", "path", "Add to the import path", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('t', "trace", "trace", "Trace the state as compiler runs", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);

    INIT_TRACE(NULL);
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    const char* fname = raw_string(get_cmd_opt("files"));
    if(fname != NULL)
        open_file(fname);
    else
        FATAL("internal error in %s: command line failed", __func__);

    token_t* tok;
    init_token_queue();
    while(true) {
        tok = get_token();
        if(tok->type == TOK_END_OF_FILE)
            break;
        fprintf(stderr, "%s \"%s\" \"%s\" %d %d\n",
                tok_type_to_str(tok), tok_type_to_str(tok),
                raw_string(tok->str), tok->line_no, tok->col_no);
        consume_token();
    }

    return 0;
}
