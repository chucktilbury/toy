/*
 *  The purpose of the phase 1 parse routines is to create the global symbol
 *  table.
 */

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "tokens.h"
#include "errors.h"
#include "parser.h"

#include "toi.h"

// need a concept of a symbol table context.
// need to add the stuff to get the file index
// need to add resolver module
// need to add token_to_str()


char *token_to_str(int tok)
{
    return "blart";
}

int resolve_for_class_name(const char *name)
{
    ENTER();
    printf("resolving class name: %s\n", name);
    return PHASE1_SUCCESS;   // class found.
}

/*
 *  The symbol and the symbol context for parameters.
 */
int do_generic_import(const char *sym)
{
    ENTER();
    DEBUG(9, "importing file for \"%s\"", sym);
    // open the file and begin a parse session on it from the root with the
    // symbol_context provided.

    return PHASE1_SUCCESS;
}

int get_generic_parameter_list(void)
{
    ENTER();

    return PHASE1_SUCCESS;
}

int get_method_parameter_input_list(void)
{
    ENTER();
    return PHASE1_SUCCESS;
}

int get_method_parameter_output_list(void)
{
    ENTER();
    return PHASE1_SUCCESS;
}

/*
 *  include name.name.name;
 *
 *  Enters phase1_parse_all() recursively.
 */
int do_import(void)
{
    ENTER();

    // next token must be either symbol_tok or complex_tok
    int token = get_token();
    switch(token)
    {
        case SYMBOL_TOK:
        case COMPLEX_TOK: do_generic_import(get_tok_str()); break;
        default:
            show_syntax_error("expected a simple file name or a path but got \'%s\'", get_tok_str());
            return PHASE1_FAILED;
        break;
    }

    // must be a ';' token
    token = get_token();
    if(token != ';')
    {
        // syntax error
        show_syntax_error("expected a \';\' but got a \"%s\'", get_tok_str());
        return PHASE1_FAILED;
    }
    else
        return PHASE1_SUCCESS;
}

/*
 *  A class parameter list is a list of SYMBOL_TOK or COMPLEX_TOK separated by
 *  by ',' and surrounded with '(' ')', which is a generic parameter list.
 */
int get_class_parameter_list(void)
{
    ENTER();

    int token;

    token = get_token();
    if(token != '(')
    {
        show_syntax_error("expected a \'(\' but got a \'%s\'", get_tok_str());
        return PHASE1_FAILED;
    }

    // get the class input parameter list
    if(get_generic_parameter_list(/* where to save it */))
        fatal_error("cannot parse class parameter list");

    token = get_token();
    if(token != ')')
    {
        show_syntax_error("expected a \')\' but got a \'%s\'", get_tok_str());
        return PHASE1_FAILED;
    }

    return PHASE1_SUCCESS;
}

/*
 *  Get the vars and the methods and index the blocks of code for future
 *  reference when we begin to execute the code. A class body is surrounded with
 *  the '{' and '}' tokens. Class bodies create the context of the class.
 */
int get_class_body(void)
{
    ENTER();

    int token;
    int finished = 0;

    token = get_token();
    if(token != '{')
    {
        show_syntax_error("expected a \'{\' but got a \'%s\'", get_tok_str());
        return PHASE1_FAILED;
    }

    // read the class body and save it
    while(!finished)
    {
        finished = 1;
    }

    token = get_token();
    if(token != '}')
    {
        show_syntax_error("expected a \'}\' but got a \'%s\'", get_tok_str());
        return PHASE1_FAILED;
    }

    return PHASE1_SUCCESS;
}


/*
 *  class name(name, name, name) { class_body }
 *
 *  The class name is a primary name. The names in the parameter list must have
 *  already been defined and must name a class.
 */
int do_class(void)
{
    ENTER();

    // first token must be a simple name
    int token = get_token();

    if(token == SYMBOL_TOK)
    {
        // create the class symbol context connected to the symbol
    }
    else
    {
        // syntax error
        show_syntax_error("pass1: class name must be simple symbol");
        return PHASE1_FAILED;   // do not continue
    }

    if(get_class_parameter_list())
        fatal_error("pass1: cannot parse input parameters");

    // get the symbols for the class
    if(get_class_body())
        fatal_error("pass1: cannot parse class body");

    return PHASE1_SUCCESS;   // success
}


/*
 *  When this is entered, the scanner is already init and the file that is to
 *  be parsed is already identified, but not opened.
 */
//int phase1_parse_all(const char *fname)
int phase1_parse_all(void)
{
    ENTER();

    int finished = 0;
    int token;

    while(!finished)
    {
        token = get_token();

        switch(token)
        {
            case IMPORT_TOK: finished = do_import(); break;
            case CLASS_TOK: finished = do_class(); break;
            // this is the only place where EOI_TOK is not an error.
            case EOI_TOK: finished++; break;
            default:
                finished = 1;
                show_syntax_error("expected include or class but got \'%s\'", token_to_str(token));
                return PHASE1_FAILED;
            break;
        }
    }

    if(get_num_errors() != 0)
        return PHASE1_FAILED;
    else
        return PHASE1_SUCCESS;
}
