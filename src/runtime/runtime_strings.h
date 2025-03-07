#ifndef _STRINGS_H_
#define _STRINGS_H_

//-----------------------------------------------------------------------------
// Strings API
// String data strucutre.
typedef struct _string_t_ {
    char* text;
    int len;
    int cap;
} runtime_string_t;

// returns an allocated string in garbage collected code.
runtime_string_t* runtime_string_format(const char*, ...);

// send the string to stdout.
void runtime_string_print(runtime_string_t*);


#endif /* _STRINGS_H_ */
