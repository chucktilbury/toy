
#ifndef _ERRORS_H_
#define _ERRORS_H_

void init_errors(FILE *stream);
void show_syntax_error(char *fmt, ...);
void show_syntax_warning(char *fmt, ...);
void fatal_error(char *fmt, ...);
int get_num_errors(void);
int get_num_warnings(void);
void set_debug_level(int level);
int get_debug_level(int level);
int show_debug_msg(const char *, int, int, char *, ...);
int show_leave_msg(const char* func, int line, char *fmt, ...);
int show_enter_msg(const char* func);
void show_info_msg(char *fmt, ...);
void show_result(void);

#ifdef DEBUGGING
#  define DEBUG(l, s, ...)  show_debug_msg(__func__, __LINE__, l, s, ##__VA_ARGS__)
#  define ENTER()           show_enter_msg(__func__)
#  define LEAVE(s, ...)     show_leave_msg(__func__, __LINE__, s, ##__VA_ARGS__)
#  define PHASE1_SUCCESS    ((LEAVE("leaving OK"))? 1:0)
#  define PHASE1_FAILED     ((LEAVE("leaving FAIL"))? 0:1)
#else
#  define DEBUG(l, s, ...)
#  define ENTER()
#  define LEAVE(s, ...)
#  define PHASE1_SUCCESS  0
#  define PHASE1_FAILED   1
#endif


#endif /* _ERRORS_H_ */
