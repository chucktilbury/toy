
#ifndef _SCANNER_H_
#define _SCANNER_H_


void open_file(char *fname);
int get_line_number(void);
char *get_file_name(void);
int get_token(void);
const char *get_tok_str(void);
int get_total_lines(void);

#endif /* _SCANNER_H_ */
