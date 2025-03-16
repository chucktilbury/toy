#ifndef _RT_STRINGS_H_
#define _RT_STRINGS_H_

//-----------------------------------------------------------------------------
// Strings API
// String data structure.
typedef struct _string_t_ {
    char* text;
    int len;
    int cap;
} rt_string_t;

// returns an allocated string in garbage collected code.
rt_string_t* rt_string_format(const char*, ...);

// send the string to stdout.
void rt_string_print(rt_string_t*);


#endif /* _RT_STRINGS_H_ */
