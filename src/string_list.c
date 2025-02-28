
#include "string_list.h"

string_list_t* create_string_list(void) {

    return (string_list_t*)create_pointer_list();
}

void destroy_string_list(string_list_t* lst) {

    destroy_pointer_list((string_list_t*)lst);
}

void add_string_list(string_list_t* lst, string_buffer_t* str) {

    add_pointer_list((pointer_list_t*)lst, (void*)str);
}

int len_string_list(string_list_t* lst) {

    return len_pointer_list((pointer_list_t*)lst);
}

string_buffer_t* index_string_list(string_list_t* lst, int idx) {

    return (string_buffer_t*)index_pointer_list((pointer_list_t*)lst, idx);
}

string_buffer_t* iterate_string_list(string_list_t* lst, int* mark) {

    return (string_buffer_t*)iterate_pointer_list((pointer_list_t*)lst, mark);
}

void push_string_list(string_list_t* lst, string_buffer_t* str) {

    push_pointer_list((pointer_list_t*)lst, (void*)str);
}

string_buffer_t* pop_string_list(string_list_t* lst) {

    return (string_buffer_t*)pop_pointer_list((pointer_list_t*)lst);
}

string_buffer_t* peek_string_list(string_list_t* lst) {

    return (string_buffer_t*)peek_pointer_list((pointer_list_t*)lst);
}

