/*
 * Interface between the parser and the scanner for scanned symbols.
 */

#include "tokens.h"
#include "memory.h"
#include "fileio.h"


token_t* create_token(const char* text, int type) {


    token_t* tok = _ALLOC_DS(token_t);
    tok->raw     = _COPY_STRING(text);
    tok->type    = type;

    tok->line_no = get_line_no();
    tok->col_no  = get_col_no();
    tok->fname   = _COPY_STRING(get_file_name());

    return tok;
}
