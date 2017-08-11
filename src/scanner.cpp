/*
 *  This is the scanner. It takes the stream of characters and breaks it up
 *  into blocks that can be identified as tokens. Some tokens are defined by
 *  this class such as numbers and strings. This class also ignores comments
 *  and white space. When this class cannot determine the token type, the string
 *  is passed to the Token class for analysis.The items handled by the Token
 *  class are mainly keywords and multi-character operators. Some operators can
 *  only appear by them selves such as "{" and "}". All tokens are returned as
 *  a Token object which contains the type and the string that was scanned.
 */
#include <iostream>
#include <string>

#include "filestack.h"
#include "logger.h"
#include "scanner.h"
#include "errors.h"


/*
// these tables contain the transitions that are valid for the state that
// the get_token routine is currently in.
_char_type_t init_table[256];
_char_type_t name_table[256];
_char_type_t dnum_table[256];
_char_type_t hnum_table[256];
_char_type_t fnum_table[256];
_char_type_t dquo_table[256];
_char_type_t squo_table[256];
 */

// scanner states
typedef enum
{
    START_STATE,
    SIMPLE_NAME_STATE,
    COMPLEX_NAME_STATE,
    DNUM_STATE,
    HNUM_STATE,
    FNUM_STATE,
    ONUM_STATE,
    DQUOTE_STATE,
    SQUOTE_STATE,
    PUNCT_STATE,
    COMMENT_STATE,
    FINISH_SNAME_STATE,
    FINISH_CNAME_STATE,
    FINISH_DNUM_STATE,
    FINISH_HNUM_STATE,
    FINISH_FNUM_STATE,
    FINISH_ONUM_STATE,
    FINISH_DQUOTE_STATE,
    FINISH_SQUOTE_STATE,
    FINISH_PUNCT_STATE,
} _scanner_state_t;

Token* Scanner::get_token(void)
{
    ENTER();
    Token* token = new Token();
    bool finished = false;
    string tok_str = string();
    int state = START_STATE;
    int ch;

    try
    {
        while(!finished)
        {
            switch(state)
            {
                case START_STATE:
                    DEBUG("START_STATE");
                    ch = filestack->get_char();
                    switch(init_table[ch])
                    {
                        // dispatch the initial state
                        case ALPHA:
                            tok_str += ch;
                            state = SIMPLE_NAME_STATE;
                            break;
                        case NUMERIC:
                            tok_str += ch;
                            state = DNUM_STATE;
                            break;
                        case PUNCT:
                            tok_str += ch;
                            state = PUNCT_STATE;
                            break;
                        case DQUOTE:
                            // do not save the quote char
                            state = DQUOTE_STATE;
                            break;
                        case SQUOTE:
                            // do not save the quote char
                            state = DQUOTE_STATE;
                            break;
                        case COMMENT:
                            // do not save the char
                            state = COMMENT_STATE;
                            break;
                        case WHITE: // do nothing is a white is encountered in start_state
                            break;

                        // error states
                        case ILLEGAL:   // not a valid printable character
                            throw ScannerError("invalid character in input");
                            break;
                        case INVALID:   // not in this table. here for safety.
                            throw ScannerError("unknown scanner state error");
                            break;
                        default:    // should be impossible
                            throw InternalScannerError("unknown scanner state error");
                            break;
                    }
                    break;
                case SIMPLE_NAME_STATE:
                    DEBUG("SIMPLE_NAME_STATE");
                    state = scan_simple_name(tok_str);
                    break;
                case COMPLEX_NAME_STATE:
                    DEBUG("COMPLEX_NAME_STATE");
                    state = scan_complex_name(tok_str);
                    break;
                case DNUM_STATE:
                    DEBUG("DNUM_STATE");
                    state = scan_dnum(tok_str);
                    break;
                case HNUM_STATE:
                    DEBUG("HNUM_STATE");
                    state = scan_hnum(tok_str);
                    break;
                case FNUM_STATE:
                    DEBUG("FNUM_STATE");
                    state = scan_fnum(tok_str);
                    break;
                case ONUM_STATE:
                    DEBUG("ONUM_STATE");
                    state = scan_onum(tok_str);
                    break;
                case DQUOTE_STATE:
                    DEBUG("DQUOTE_STATE");
                    state = scan_dquote(tok_str);
                    break;
                case SQUOTE_STATE:
                    DEBUG("SQUOTE_STATE");
                    state = scan_squote(tok_str);
                    break;
                case PUNCT_STATE:
                    DEBUG("PUNCT_STATE");
                    state = scan_punct(tok_str);
                    break;
                case COMMENT_STATE:
                    DEBUG("COMMENT_STATE");
                    state = scan_comment(tok_str);
                    break;

                // finish states
                case FINISH_PUNCT_STATE:
                    // look up the token and return the Token object
                    DEBUG("FINISH_PUNCT_STATE");
                    {
                        int type = token->lookup_operator(tok_str.c_str());
                        if(type != -1)
                        {
                            token->set_type(type);
                            token->set_str(tok_str);
                        }
                        else
                            SyntaxError("unknown operator: %s", tok_str.c_str());
                    }
                    finished = true;
                    break;
                case FINISH_SNAME_STATE:
                    // look up the token and return the Token object
                    DEBUG("FINISH_SNAME_STATE");
                    {
                        int type = token->lookup_keyword(tok_str.c_str());
                        if(type == -1)
                            token->set_type(SNAME_TOK);
                        else
                            token->set_type(type);
                        token->set_str(tok_str);

                    }
                    finished = true;
                    break;
                case FINISH_CNAME_STATE:
                    // look up the token and return the Token object
                    DEBUG("FINISH_CNAME_STATE");
                    token->set_type(CNAME_TOK);
                    token->set_str(tok_str);
                    finished = true;
                    break;
                case FINISH_DNUM_STATE:
                    // return the token with the value set
                    DEBUG("FINISH_DNUM_STATE");
                    token->set_type(SNUM_TOK);
                    token->set_snum(stoll(tok_str, NULL, 10));
                    finished = true;
                    break;
                case FINISH_HNUM_STATE:
                    // return the token with the value set
                    DEBUG("FINISH_HNUM_STATE");
                    token->set_type(UNUM_TOK);
                    token->set_unum(stoull(tok_str, NULL, 16));
                    finished = true;
                    break;
                case FINISH_FNUM_STATE:
                    // return the token with the value set
                    DEBUG("FINISH_FNUM_STATE");
                    token->set_type(FNUM_TOK);
                    token->set_fnum(stod(tok_str, NULL));
                    finished = true;
                    break;
                case FINISH_ONUM_STATE:
                    // return the token with the value set
                    DEBUG("FINISH_ONUM_STATE");
                    token->set_type(UNUM_TOK);
                    token->set_unum(stoull(tok_str, NULL, 8));
                    finished = true;
                    break;
                case FINISH_DQUOTE_STATE:
                    DEBUG("FINISH_DQUOTE_STATE");
                    token->set_type(LSTRG_TOK);
                    token->set_str(tok_str);
                    finished = true;
                    break;
                case FINISH_SQUOTE_STATE:
                    DEBUG("FINISH_SQUOTE_STATE");
                    token->set_type(LSTRG_TOK);
                    token->set_str(tok_str);
                    finished = true;
                    break;
                default:    // should be impossible
                    DEBUG("scanner error");
                    throw InternalScannerError("unknown scanner state error");
                    break;
            }
        }
    }
    catch (EndOfInput &e)
    {
        // not an error. Create and return the EOI token
        delete token;
        token = nullptr;
    }

    RETURN(token, "token: %s", (token!=nullptr)?token->print():"EOI");
}

/*
 *  When this is entered, we already have the first character as an ALPHA.
 */
int Scanner::scan_simple_name(string& tok_str)
{
    int ch;
    bool finished = false;
    //int state = 0;
    int retv = FINISH_SNAME_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case WHITE:
                case PUNCT:
                    // end the string and return it
                    filestack->unget_char(ch);
                    finished = true;
                    retv = FINISH_SNAME_STATE;
                    break;
                case ALPHA:
                case NUMERIC:
                    // add the character to the string and continue
                    tok_str += ch;
                    break;
                case DOT:
                    // have a complex name
                    tok_str += ch;
                    finished = true;
                    retv = COMPLEX_NAME_STATE;
                    break;
                default:
                    // Everything else is a syntax error
                    tok_str += ch;
                    throw SyntaxError("malformed name: %s", tok_str.c_str());
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        throw SyntaxError("unexpected end of file");
    }
    return retv;
}

int Scanner::scan_complex_name(string& tok_str)
{
    int ch;
    bool finished = false;
    //int state = 0;
    int retv = FINISH_CNAME_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case WHITE:
                case PUNCT:
                    // end the string and return it
                    filestack->unget_char(ch);
                    finished = true;
                    retv = FINISH_CNAME_STATE;
                    break;
                case DOT:
                case ALPHA:
                case NUMERIC:
                    // add the character to the string and continue
                    tok_str += ch;
                    break;
                default:
                    // Everything else is a syntax error
                    tok_str += ch;
                    throw SyntaxError("malformed name: %s", tok_str.c_str());
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        throw SyntaxError("unexpected end of file");
    }
    return retv;
}

/*
 *  Note that this function could be entered from another outer state with the
 *  string already having contents.
 */
int Scanner::scan_dnum(string& tok_str)
{
    int ch, state = 0;
    bool finished = false;
    int retv = FINISH_DNUM_STATE;

    // In the end, we only care if a number is signed, unsigned or float. The
    // size of a number is only interesting at the application level and is
    // determined by the value it has.
    try
    {
        while(!finished)
        {
            switch(state)
            {
                case 0:
                    // Starting state. We could have a '0' at the beginning of the
                    // string. If we do, then we could be scanning an octal number,
                    // a hex number, or a float. If it is not a '0', then we are
                    // scanning a "regular" number for now, but that could change.
                    ch = tok_str.c_str()[0];
                    switch(ch)
                    {
                        case '0':
                            // type of numeric literal is not known yet, switch
                            // to state 1.
                            state = 1;
                            break;
                        default:
                            // assume a decimal nmumber
                            state = 2;
                            break;
                    }
                    break;
                case 1:
                    // Get the second character. If it's in [xX] then switch to the
                    // hex number scanner in the outer loop. If it's a '.', then we
                    // have a float. If it's in [0-7] then it could be an octal, so
                    // switch to the octal number scanner in the outer loop. numbers
                    // that start with a 0 but are decimal are permissable.
                    ch = filestack->get_char();
                    switch(init_table[ch])
                    {
                        case PUNCT:
                        case WHITE:
                            // finished.
                            filestack->unget_char(ch);
                            retv = FINISH_DNUM_STATE;
                            finished = true;
                            break;
                        case ALPHA:
                            // could have an 'x', otherwise this is a malformed symbol
                            if(ch == 'x' || ch == 'X')
                            {
                                tok_str += ch;
                                retv = HNUM_STATE;
                                finished = true;
                            }
                            else
                            {
                                // syntax error
                                tok_str += ch;
                                throw SyntaxError("malformed number: %s", tok_str.c_str());
                            }
                            break;
                        case DOT:
                            // if we have a dot then it's a float, change states
                            tok_str += ch;
                            retv = FNUM_STATE;
                            finished = true;
                            break;
                        case NUMERIC:
                            // if it fits into an octal digit, then switch,
                            // otherwise just copy the char and switch to state 2.
                            switch(ch)
                            {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                    tok_str += ch;
                                    retv = ONUM_STATE;
                                    finished = true;
                                    break;
                                default:
                                    tok_str += ch;
                                    state = 2;  // assume a decimal
                            }
                            break;
                        default:
                            // everything else is a syntax error
                            tok_str += ch;
                            throw SyntaxError("malformed number: %s", tok_str.c_str());
                            break;
                    }
                    break;
                case 2:
                    // Scanning a decimal number. If we scan a '.' then it's
                    // actually a float so we switch to the outer loop and that
                    // scanner.
                    ch = filestack->get_char();
                    switch(init_table[ch])
                    {
                        case NUMERIC:
                            // save the character and continue
                            tok_str += ch;
                            break;
                        case PUNCT:
                        case COMMENT:
                        case WHITE:
                            // finished scanning the number
                            filestack->unget_char(ch);
                            finished = true;
                            retv = FINISH_DNUM_STATE; // documentation
                            break;
                        case DOT:
                            // have a float, switch to that state.
                            tok_str += ch;
                            retv = FNUM_STATE;
                            finished = true;
                            break;
                        default:
                            // everything else is treated as a malformed number
                            // error
                            tok_str += ch;
                            throw ScannerError("malformed decimal number: %s", tok_str.c_str());
                            break;
                    }
                    break;
                default:
                    // internal error. If this happens the interpreter is screwed.
                    throw InternalScannerError("invalid state in %s", __func__);
                    break;
            }
        }
    }
    catch(EndOfInput &e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

int Scanner::scan_onum(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_ONUM_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case NUMERIC:
                    // numeric, make sure it's an octal digit.
                    switch(ch)
                    {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                            // add the character and continue
                            tok_str += ch;
                            break;
                        default:
                            // not an octal. Switch to the decimal number state
                            finished = true;
                            tok_str += ch;
                            retv = DNUM_STATE;
                            break;
                    }

                    break;
                case PUNCT:
                case COMMENT:
                case WHITE:
                    // finish the octal number
                    finished = true;
                    filestack->unget_char(ch);
                    retv = FINISH_ONUM_STATE;
                    break;
                case DOT:
                    // we have a float that happens to not contain a all octal
                    // digits. switch to the outer loop and get the float.
                    finished = true;
                    tok_str += ch;
                    retv = FNUM_STATE;
                    break;
                default:
                    // malformed number error
                    tok_str += ch;
                    throw ScannerError("malformed octal number: %s", tok_str.c_str());
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }
    return retv;
}

/*
 *  When this is entered, the tok_str contains "0x".
 */
int Scanner::scan_hnum(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_HNUM_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case PUNCT:
                case WHITE:
                    // finished with the number
                    filestack->unget_char(ch);
                    finished = true;
                    retv = FINISH_HNUM_STATE;
                    break;
                case NUMERIC:
                    // simply add the char
                    tok_str += ch;
                    break;
                case ALPHA:
                    // verify that the char is >='a'&&<='f'||>='A'&&<='F'
                    if((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
                    {
                        // just add the character
                        tok_str += ch;
                    }
                    else
                    {
                        // syntax error
                        tok_str += ch;
                        SyntaxError("malformed hex number: %s", tok_str.c_str());
                    }
                    break;
                default:
                    // everything else is a syntax error
                    tok_str += ch;
                    SyntaxError("malformed hex number: %s", tok_str.c_str());
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

/*
 *  This string will always have something in it. Usually this state is reached
 *  from the DNUM state. Exponent syntax is not scanned.
 */
int Scanner::scan_fnum(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_HNUM_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case PUNCT:
                case WHITE:
                    // finished with the number
                    filestack->unget_char(ch);
                    finished = true;
                    retv = FINISH_FNUM_STATE;
                    break;
                case NUMERIC:
                    // simply add the char
                    tok_str += ch;
                    break;
                case DOT:
                    SyntaxError("float can contain only one dot");
                    break;
                default:
                    // everything else is a syntax error
                    tok_str += ch;
                    SyntaxError("malformed float number: %s", tok_str.c_str());
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

void Scanner::handle_escape(string& tok_str)
{

}

/*
 *  This scans a quoted string with escapes.
 */
int Scanner::scan_dquote(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_DQUOTE_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case DQUOTE:
                    // finish with the string. do not add the quote char.
                    finished = true;
                    retv = FINISH_DQUOTE_STATE;
                    break;
                case WHITE:
                    // need to make sure there is no '\r' or '\n', otherwise,
                    // just add it to the string. A newline is a syntax error.
                    if(ch == '\n' || ch == '\r')
                    {
                        SyntaxError("unexpected new line in quoted string");
                    }
                    else
                    {
                        tok_str += ch;
                    }
                    break;
                case PUNCT:
                    // If there is a '\\' then handle the escape, otherwise just
                    // add it to the string.
                    if(ch == '\\')
                    {
                        handle_escape(tok_str);
                    }
                    else
                    {
                        tok_str += ch;
                    }
                    break;
                default:
                    // just add everything to the string
                    tok_str += ch;
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

/*
 *  This scans a quoted string as literal.
 */
int Scanner::scan_squote(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_SQUOTE_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case SQUOTE:
                    // finish with the string. do not add the quote char.
                    finished = true;
                    retv = FINISH_SQUOTE_STATE;
                    break;
                case WHITE:
                    // need to make sure there is no '\r' or '\n', otherwise,
                    // just add it to the string. A newline is a syntax error.
                    if(ch == '\n' || ch == '\r')
                    {
                        SyntaxError("unexpected new line in quoted string");
                    }
                    else
                    {
                        tok_str += ch;
                    }
                    break;
                default:
                    // just add everything to the string
                    tok_str += ch;
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

int Scanner::scan_punct(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = FINISH_PUNCT_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case PUNCT:
                    // finish with the string. do not add the quote char.
                    tok_str += ch;
                    break;
                default:
                    // just add everything to the string
                    filestack->unget_char(ch);
                    finished = true;
                    retv = FINISH_PUNCT_STATE;
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        throw SyntaxError("unexpected end of file");
    }

    return retv;
}

/*
 *  The simplest one.
 */
int Scanner::scan_comment(string& tok_str)
{
    int ch;
    bool finished = false;
    int retv = START_STATE;

    try
    {
        while(!finished)
        {
            ch = filestack->get_char();
            switch(init_table[ch])
            {
                case WHITE:
                    if(ch == '\n' || ch == '\r')
                    {
                        finished = true;
                        retv = START_STATE;
                    }
                    break;
                default:
                    // ignore everything
                    break;
            }
        }
    }
    catch (EndOfInput& e)
    {
        // end of input is an error here.
        // NOTE that the EOI is not really a syntax error here. The read char
        // function continues to read EOI after the first one, so it will
        // propigate back to the outer loop.
        // throw SyntaxError("unexpected end of file");
    }

    return retv;
}


Scanner::Scanner(void)
{
    ENTER();
    const char *ops = "~!@$%^&*()+-={}[]\\|:;,<>/?";

    token = new Token();
    filestack = new FileStack();

    // it's possible to access the filestack before it has been created.
    // This prevents that.
    filestack_created ++;

    // init the character table
    for(int i = 0; i < 256; i++)
        init_table[i] = ILLEGAL;
    for(int i = 0; ops[i] != 0; i++)
        init_table[(int)ops[i]] = PUNCT;
    for(int i = 'a'; i <= 'z'; i++)
        init_table[i] = ALPHA;
    for(int i = 'A'; i <= 'Z'; i++)
        init_table[i] = ALPHA;
    for(int i = '0'; i <= '9'; i++)
        init_table[i] = NUMERIC;
    init_table['_'] = ALPHA;
    init_table['\"'] = DQUOTE;
    init_table['\''] = SQUOTE;
    init_table['#'] = COMMENT;
    init_table['.'] = DOT;
    init_table[' '] = WHITE;
    init_table['\r'] = WHITE;
    init_table['\n'] = WHITE;
    init_table['\t'] = WHITE;

    LEAVE();
}

Scanner::~Scanner(void)
{
    ENTER();
    delete filestack;
    delete token;
    LEAVE();
}

bool Scanner::open_file(const string fname)
{
    return filestack->open(fname);
}

void Scanner::unget_char(char ch)
{
    ENTER();
    filestack->unget_char(ch);
    VOID_RET("some string");
}

// these return the CURRENT information and may be called from a location
// that does not have ready accesss to the scanner object.
const string* Scanner::filename(void)
{
    if(filestack_created)
        return filestack->filename();
    else
        return nullptr;
}

int Scanner::lineno(void)
{
    if(filestack_created)
        return filestack->lineno();
    else
        return -1;
}

int Scanner::total_lines(void) const
{
    if(filestack_created)
        return filestack->total_lines();
    else
        return -1;
}
