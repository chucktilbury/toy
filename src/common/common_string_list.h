#ifndef _COMMON_STRING_LIST_
#define _COMMON_STRING_LIST_

#include "common_string_buffer.h"
#include "common_pointer_list.h"

typedef pointer_list_t string_list_t;

string_list_t* create_string_list(void);
void destroy_string_list(string_list_t*);
void add_string_list(string_list_t*, string_buffer_t*);
int len_string_list(string_list_t*);
string_buffer_t* index_string_list(string_list_t*, int);
string_buffer_t* iterate_string_list(string_list_t*, int*);

void push_string_list(string_list_t*, string_buffer_t*);
string_buffer_t* pop_string_list(string_list_t*);
string_buffer_t* peek_string_list(string_list_t*);


#endif /* _COMMON_STRING_LIST_ */
