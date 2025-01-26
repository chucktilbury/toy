#ifndef _FILEIO_H_
#define _FILEIO_H_

// these functions are defined in scanner.l
void open_file(const char* fname);
int get_line_no(void);
int get_col_no(void);
const char* get_file_name(void);

#endif /* _FILEIO_H_ */
