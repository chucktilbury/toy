/**
 * @file runtime.h
 *
 * @brief
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "rt_object.h"
#include "rt_memory.h"
#include "rt_strings.h"
#include "rt_errors.h"

typedef struct _rt_call_stack_t_ {
    const char* name;   // function prototype
    const char* fname;  // file name where the function is defined
    int line;           // line number where the function is defined.
    rt_gc_node_t* node; // garbage collection node
    struct _rt_call_stack_t_* next;
} rt_call_stack_t;

// initialize the runtime code
void rt_main_init(int, char**, char**);

// clean up after program run
void rt_main_uninit(void);

// Enter a function
void rt_enter_func(const char* name, const char* fname, int line);

// Leave a function
void rt_leave_func(void);

#endif /* _RUNTIME_H_ */
