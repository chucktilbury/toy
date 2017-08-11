/*
 *  This is the lexer. It reads a stream of characters from the input stream
 *  and breaks it up into words. In general, white space is ignored. Strings
 *  are "cooked" if the are enclosed in "" and "raw" if they are enclosed in
 *  ''. For cooked strings, escapes such as "\n" and \x20 are honored.
 *
 *  This scanner works by the notion of similar characters. It uses several
 *  tables of characters to figure out what that means. In general, if alpha-
 *  numeric characters are being scanned, then a word is decided by whitespace
 *  or something that looks like an operator. Then the chunk is passed on to the
 *  token generator to decide what the token value of the chunk is.
 */
#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

#include "filestack.h"
#include "token.h"

using namespace std;

// Character types that the scanner uses.
typedef enum
{
    INVALID,    // not white space, not printable
    ALPHA,      // the set [a-z][A-Z]
    NUMERIC,    // the set [0-9]
    PUNCT,      // the set of "~`!@$%^&*()+-={}[]\|:;,<>/?"
    WHITE,      // the set of ' '\n\t\f\r
    DQUOTE,     // the \" character
    SQUOTE,     // the \' character
    DOT,        // the \. character
    COMMENT,    // the \# character
    ESCAPE,
    VALID,
    ILLEGAL,
} _char_type_t;

class Scanner
{
public:
    Scanner(void);
    ~Scanner(void);
    bool open_file(const string fname);
    Token* get_token(void);
    void unget_char(char ch);
    int total_lines(void) const;

    // these return the CURRENT information and may be called from a location
    // that does not have ready accesss to the scanner object.
    static const string* filename(void);
    static int lineno(void);
    static const string token_string(void);

private:
    static FileStack* filestack;
    static int filestack_created;
    Token* token;
    int tot_lines;

    // scanner state handlers
    int scan_simple_name(string& tok_str);
    int scan_complex_name(string& tok_str);
    int scan_dnum(string& tok_str);
    int scan_hnum(string& tok_str);
    int scan_fnum(string& tok_str);
    int scan_onum(string& tok_str);
    int scan_dquote(string& tok_str);
    int scan_squote(string& tok_str);
    int scan_punct(string& tok_str);
    int scan_comment(string& tok_str);
    void handle_escape(string& tok_str);

    // these tables contain the transitions that are valid for the state that
    // the get_token routine is currently in.
    _char_type_t init_table[256];
};


#endif /* !_TOKEN_H_ */
