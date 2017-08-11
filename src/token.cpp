
#include "token.h"

// This struct is private data to the Token class.
// This is list is sorted lexically for a binary search.
// Command to sort the array in bash
//      "cat infile.txt | LC_ALL=C sort > outfile.txt"
static _token_name_t _word_token_list[] =
{
    {"and"          , AND_TOK},
    {"array"        , ARRAY_TOK},
    {"at"           , AT_TOK},
    {"bool"         , BOOL_TOK},
    {"break"        , BREAK_TOK},
    {"case"         , CASE_TOK},
    {"class"        , CLASS_TOK},
    {"const"        , CONST_TOK},
    {"continue"     , CONTINUE_TOK},
    {"create"       , CREATE_TOK},
    {"destroy"      , DESTROY_TOK},
    {"dict"         , DICT_TOK},
    {"do"           , DO_TOK},
    {"else"         , ELSE_TOK},
    {"elseif"       , ELSEIF_TOK},
    {"except"       , EXCEPT_TOK},
    {"false"        , FALSE_TOK},
    {"float"        , FLOAT_TOK},
    {"for"          , FOR_TOK},
    {"if"           , IF_TOK},
    {"import"       , IMPORT_TOK},
    {"int"          , INT_TOK},
    {"int16"        , INT16_TOK},
    {"int32"        , INT32_TOK},
    {"int64"        , INT64_TOK},
    {"int8"         , INT8_TOK},
    {"is"           , IS_TOK},
    {"isnot"        , ISNOT_TOK},
    {"locate"       , LOCATE_TOK},
    {"method"       , METHOD_TOK},
    {"not"          , NOT_TOK},
    {"nothing"      , NOTHING_TOK},
    {"or"           , OR_TOK},
    {"private"      , PRIVATE_TOK},
    {"protected"    , PROTECTED_TOK},
    {"public"       , PUBLIC_TOK},
    {"raise"        , RAISE_TOK},
    {"static"       , STATIC_TOK},
    {"string"       , STRING_TOK},
    {"switch"       , SWITCH_TOK},
    {"true"         , TRUE_TOK},
    {"uint"         , UINT_TOK},
    {"uint16"       , UINT16_TOK},
    {"uint32"       , UINT32_TOK},
    {"uint64"       , UINT64_TOK},
    {"uint8"        , UINT8_TOK},
    {"while"        , WHILE_TOK},

    // mark the last one, but the binary search does not use it
    {NULL, -1}
};

static _token_name_t _operator_token_list[] =
{
    {"!"            , NOT_TOK},
    {"!="           , NEQU_TOK},
    {"$"            , '$'},
    {"%"            , '%'},
    {"&"            , '&'},
    {"&&"           , AND_TOK},
    {"("            , '('},
    {")"            , ')'},
    {"*"            , '*'},
    {"+"            , '+'},
    {","            , ','},
    {"-"            , '-'},
    {"."            , '.'},
    {"/"            , '/'},
    {":"            , ':'},
    {";"            , ';'},
    {"<"            , LESS_TOK},
    {"<<"           , SHL_TOK},
    {"<="           , LTEQU_TOK},
    {"="            , '='},
    {"=="           , EQU_TOK},
    {">"            , MORE_TOK},
    {">="           , GTEQU_TOK},
    {">>"           , SHR_TOK},
    {"?"            , '?'},
    {"@"            , '@'},
    {"["            , '['},
    {"]"            , ']'},
    {"^"            , '^'},
    {"{"            , '{'},
    {"|"            , '|'},
    {"||"           , OR_TOK},
    {"}"            , '}'},
    {"~"            , '~'},

    // mark the last one, but the binary search does not use it
    {NULL, -1}
};
static const int number_of_word_tokens = sizeof(_word_token_list)/sizeof(_token_name_t)-1;
static const int number_of_oper_tokens = sizeof(_operator_token_list)/sizeof(_token_name_t)-1;

int Token::lookup_keyword(const char* str)
{
    int val = binary_lookup(_word_token_list, str, 0, number_of_word_tokens-1);

    if(val >= 0)
        return _word_token_list[val].type;
    else
        return -1;
}

// return the type that the string represents, or -1 if it does not represent a
// token from the list.
int Token::lookup_operator(const char* str)
{
    int val = binary_lookup(_operator_token_list, str, 0, number_of_oper_tokens-1);

    if(val >= 0)
        return _operator_token_list[val].type;
    else
        return -1;
}

// return the index of the element found, or -1.
int Token::binary_lookup(_token_name_t list[], const char* str, int first, int last)
{
    //printf("%d:%d\n", first, last);
    if(first <= last)
    {
        int mid = first + ((last - first) / 2);
        int comp = strcmp(str, list[mid].str);

        if(comp == 0)
            // look at the middle item and if it matches, return
            return mid;
        else if(comp > 0)
            // else if it's greater, then recurse with the right half
            return binary_lookup(list, str, mid+1, last);
        else
            // else recurse with the left half
            return binary_lookup(list, str, first, mid-1);
    }
    else
    {
        return -1;
    }
}
