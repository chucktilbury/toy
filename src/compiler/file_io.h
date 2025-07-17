#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include "string_buffer.h"

void open_file(const char* name, const char* ext);
void close_file(void);
int get_char(void);
int get_line_no(void);
int get_col_no(void);
const string_t* get_file_name(void);
void update_numbers(void);

#endif /* _FILE_IO_H_ */
