
#ifndef _ERRORS_H_
#define _ERRORS_H_

void init_errors(FILE *stream);
void show_syntax_error(char *fmt, ...);
void show_syntax_warning(char *fmt, ...);
void show_fatal_error(char *fmt, ...);
int get_num_errors(void);
int get_num_warnings(void);
void set_debug_level(int level);
void show_debug_msg(int level, char *fmt, ...);
void show_info_msg(char *fmt, ...);

#endif /* _ERRORS_H_ */
