
#ifndef _ERRORS_H_
#define _ERRORS_H_
#if 0
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
#endif

#include <exception>
#include <string>
#include <cstdarg>
#include <cstring>
#include <cerrno>
using namespace std;

class ParserException
{

    public:
    static int num_errors;
    static int num_warnings;

    ParserException() {}

    ParserException(const char* str)
    {
        strncpy((char*)message_buffer, str, sizeof(message_buffer));
    }

    ParserException(const string fmt, ...)
    {
        sprintf((char*)message_buffer, "Parser error: ");
        char* ptr = (char*)&message_buffer[strlen(message_buffer)];

        va_list args;
        va_start(args, fmt);
        vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
        va_end(args);
    }

    // let the handler handle showing the message
    const char* msg() const throw()
    {
        return message_buffer;
    }

    protected:
    char message_buffer[1024];

};

int ParserException::num_errors;
int ParserException::num_warnings;

class EndOfInput: public exception, public ParserException
{
    public:
        EndOfInput(void)
        {
            sprintf((char*)message_buffer, "end of input");
        }
};

class MemoryError: public exception, public ParserException
{
    public:
        MemoryError(const string fmt, ...)
        {
            num_errors++;

            sprintf((char*)message_buffer, "Memory allocation error: ");
            char* ptr = (char*)&message_buffer[strlen(message_buffer)];

            va_list args;
            va_start(args, fmt);
            vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
            va_end(args);
        }
};

class FileError: public exception, public ParserException
{
    public:
        FileError(const string fname, const string err)
        {
            snprintf((char*)message_buffer, sizeof(message_buffer),
                     "File error: cannot open file \"%s\": %s\n", fname.c_str(), err.c_str());
        }
};

class SyntaxError: public exception, public ParserException
{
    public:
    SyntaxError(const string fmt, ...)
    {
        //inc_error();
        num_errors++;

        sprintf((char*)message_buffer, "Syntax error: ");
        char* ptr = (char*)&message_buffer[strlen(message_buffer)];

        va_list args;
        va_start(args, fmt);
        vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
        va_end(args);
    }
};

class SyntaxWarning: public exception, public ParserException
{
    public:
    SyntaxWarning(const string fmt, ...)
    {
        //inc_error();
        num_warnings++;

        sprintf((char*)message_buffer, "Syntax error: ");
        char* ptr = (char*)&message_buffer[strlen(message_buffer)];

        va_list args;
        va_start(args, fmt);
        vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
        va_end(args);
    }
};


#endif /* _ERRORS_H_ */
