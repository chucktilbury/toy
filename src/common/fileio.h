
#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdio.h>
#include <stdbool.h>
#include "string_buffer.h"

const char* find_file(const char* fname, const char* ext);
const char* get_base_file_name(void);
void set_base_file_name(const char*);

#endif /* _FILEIO_H_ */
