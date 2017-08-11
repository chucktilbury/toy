/*
 *  This implements the part of the lexer that turns a string into a token. It
 *  requires the lexer to retrieve the strings from the input stream.
 */
#ifndef _TOKEN_CLASS_H_
#define _TOKEN_CLASS_H_

typedef struct _token_name
{
    const char* str;
    int type;
} _token_name_t;


// Do not want to create this data every time a Token is created.
// Performance issue.

// Define the token values. Must be greater than a character value.
// Token values are used by the parser, so they are in the header.
typedef enum
{
    AND_TOK = 300,
    ARRAY_TOK,
    AT_TOK,
    BOOL_TOK,
    BREAK_TOK,
    CASE_TOK,
    CLASS_TOK,
    CONST_TOK,
    CONTINUE_TOK,
    CREATE_TOK,
    DESTROY_TOK,
    DICT_TOK,
    DO_TOK,
    ELSEIF_TOK,
    ELSE_TOK,
    EQU_TOK,
    EXCEPT_TOK,
    FALSE_TOK,
    FLOAT_TOK,
    FOR_TOK,
    GTEQU_TOK,
    IF_TOK,
    IMPORT_TOK,
    INT16_TOK,
    INT32_TOK,
    INT64_TOK,
    INT8_TOK,
    INT_TOK,
    ISNOT_TOK,
    IS_TOK,
    LESS_TOK,
    LOCATE_TOK,
    LTEQU_TOK,
    METHOD_TOK,
    MORE_TOK,
    NEQU_TOK,
    NOTHING_TOK,
    NOT_TOK,
    OR_TOK,
    PRIVATE_TOK,
    PROTECTED_TOK,
    PUBLIC_TOK,
    RAISE_TOK,
    SHL_TOK,
    SHR_TOK,
    STATIC_TOK,
    STRING_TOK,
    SWITCH_TOK,
    TRUE_TOK,
    UINT16_TOK,
    UINT32_TOK,
    UINT64_TOK,
    UINT8_TOK,
    UINT_TOK,
    WHILE_TOK,

    // token types that are not constructed from a literal string
    SNAME_TOK,
    CNAME_TOK,
    UNUM_TOK,
    SNUM_TOK,
    FNUM_TOK,
    LSTRG_TOK,  // literal string

} _token_value_t;

#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class Token
{
    public:

    Token(void)
    {
        memset(word_buffer, 0, sizeof(word_buffer));
    }

    Token(string str)
    {
        char *strg = (char*)str.c_str();
        unsigned int i;
        for(i = 0; i < sizeof(word_buffer)-1 && strg[i] != 0; i++)
            word_buffer[i] = strg[i];
        word_buffer[i] = 0;
    }

    // These methods are used to look up the word in the word buffer to see if
    // it's a keyword. If it is, then return that value, otherwise return -1.
    int lookup_keyword(const char* str);
    int lookup_operator(const char* str);

    // value getters and setters
    // get/set token type
    int get_type(void) { return type; }
    void set_type(int t) { type = t; }

    // get/set token values
    void set_str(string str)
    {
        char *strg = (char*)str.c_str();
        unsigned int i;
        for(i = 0; i < sizeof(value.str)-1 && strg[i] != 0; i++)
            value.str[i] = strg[i];
        value.str[i] = 0;
    }
    string get_str(void) { return string(value.str); }

    void set_unum(unsigned long long num) { value.unum = num; }
    unsigned long long get_unum(void) { return value.unum; }
    void set_snum(long long num) { value.snum = num; }
    long long get_snum(void) { return value.snum; }
    void set_fnum(double num) { value.fnum = num; }
    double get_fnum(void) { return value.fnum; }
    const char* print(void)
    {
        switch(type)
        {
            case SNUM_TOK:
                sprintf(word_buffer, "%d: %lld", type, value.snum);
                break;
            case UNUM_TOK:
                sprintf(word_buffer, "%d: 0x%llX", type, value.unum);
                break;
            case FNUM_TOK:
                sprintf(word_buffer, "%d: %f", type, value.fnum);
                break;
            default:
                sprintf(word_buffer, "%d: %s", type, value.str);
                break;
        }
        return word_buffer;
    }

    private:
    union
    {
        long long int snum;
        unsigned long long int unum;
        double fnum;
        // do not want a class in my union.
        char str[1024];
    } value;
    int type;
    char word_buffer[1024];
    int binary_lookup(_token_name_t list[], const char* str, int first, int last);

};

#endif /* !_TOKEN_CLASS_H_ */
