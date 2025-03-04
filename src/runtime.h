#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "string_buffer.h"

extern int TOY_RETV;

void runtime_main_init(int, char**, char**);
const char* runtime_string_format(const char* fmt, ...);
void runtime_iterator_preamble(void);
void runtime_iterator_postamble(void);

#endif /* _RUNTIME_H_ */
