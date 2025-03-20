#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "rt_memory.h"
#include "rt_strings.h"
#include "rt_object.h"
#include "rt_errors.h"

typedef struct _rt_call_stack_t_ {
    const char* name;
    int line;
    struct _rt_call_stack_t_* next;
    rt_gc_node_t* node;
} rt_call_stack_t;

// initialize the runtime code
void rt_main_init(int, char**, char**);

// clean up after program run
void rt_main_uninit(void);

// Enter a function
void rt_enter_func(const char* name, int line);

// Leave a function
void rt_leave_func(void);

#endif /* _RUNTIME_H_ */
