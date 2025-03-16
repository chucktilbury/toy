#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "rt_memory.h"
#include "rt_strings.h"
#include "rt_object.h"


extern int rt_error_number;

// initialize the runtime code
void rt_main_init(int, char**, char**);

// clean up after program run
void rt_main_uninit(void);




#endif /* _RUNTIME_H_ */
