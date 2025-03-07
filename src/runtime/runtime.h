#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "runtime_memory.h"
#include "runtime_strings.h"
#include "runtime_object.h"


extern int runtime_error_number;

// initialize the runtime code
void runtime_main_init(int, char**, char**);

// clean up after program run
void runtime_main_uninit(void);




#endif /* _RUNTIME_H_ */
