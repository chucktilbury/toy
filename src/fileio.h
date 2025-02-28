#ifndef _FILEIO_H_
#define _FILEIO_H_

// These functions are defined in scanner.l
void open_file(const char* fname);
int get_line_no(void);
int get_col_no(void);
const char* get_file_name(void);

// These are defined in fileio.c
const char* find_file(const char* fname);
const char* get_base_file_name(void);
void set_base_file_name(const char*);

#endif /* _FILEIO_H_ */
