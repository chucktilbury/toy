
#ifndef _ERRORS_H_
#define _ERRORS_H_

void init_errors(FILE *stream);
void show_syntax_error(char *fmt, ...);
void show_syntax_warning(char *fmt, ...);
void fatal_error(char *fmt, ...);
int get_num_errors(void);
int get_num_warnings(void);
void set_debug_level(int level);
void show_debug_msg(const char *, int, int, char *, ...);
void show_info_msg(char *fmt, ...);

#ifdef DEBUGGING
#define DEBUG(l, s, ...)   show_debug_msg(__func__, __LINE__, l, s, ##__VA_ARGS__)
#else
#define DEBUG(l, s, ...)
#endif


#endif /* _ERRORS_H_ */