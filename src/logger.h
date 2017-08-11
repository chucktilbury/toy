/*
 *  This file implements logging. If the compile time var "LOGGING" is not
 *  provided then all of this simply dissappears from the output. If LOGGING
 *  is present, then the logging level can be set by class, method, and globally
 *  to control the amount of spew that it seen.
 */
#ifndef _TOI_LOGGER_H_
#define _TOI_LOGGER_H_

#include <cstdarg>
#include <cstring>
#include <iostream>
using namespace std;

// use these handy macros, not the logger directly.
#ifdef LOGGING
#   define LOG(l, s, ...)   do{Logger::logger(__func__, __LINE__, 3, s, ##__VA_ARGS__);}while(0)
#   define DEBUG(s, ...)    do{Logger::logger(__func__, __LINE__, 4, s, ##__VA_ARGS__);}while(0)
#   define WARNING(s, ...)  do{Logger::logger(__func__, __LINE__, 2, s, ##__VA_ARGS__);}while(0)
#   define ERROR(s, ...)    do{Logger::logger(__func__, __LINE__, 1, s, ##__VA_ARGS__);}while(0)
#   define ENTER()          do{Logger::enter(__func__);}while(0)
#   define LEAVE()          do{Logger::leave(__func__, __LINE__);}while(0)
#   define RETURN(v, s, ...)    do{Logger::leave(__func__, __LINE__, s, ##__VA_ARGS__); return(v);}while(0)
#   define VOID_RET(s, ...) do{Logger::leave(__func__, __LINE__, s, ##__VA_ARGS__); return;}while(0)
#   define PUSH_LOG_LEVEL(v)    do{Logger::push_level(v);}while(0)
#   define POP_LOG_LEVEL(v)     do{Logger::pop_level(v);}while(0)

class Logger
{
public:

    Logger()
    {
        //log_file = cout; //log_stream;
        push_level(0);
    }

    Logger(int lev)
    {
        //log_file = cout; //log_stream;
        push_level(lev);
    }

    static void enter(const char* func)
    {
        if(level_stack[lstack_idx] >= 4)
        {
            cout << "Enter: " << func << "\n";
        }
    }

    static int leave(const char *func, int line, const char* fmt, ...)
    {
        if(level_stack[lstack_idx] >= 4)
        {
            va_list args;
            va_start(args, fmt);
            vsnprintf(logging_buffer, sizeof(logging_buffer), fmt, args);
            va_end(args);

            cout << "Leave: " << func << ": "<< line << ": " << logging_buffer << "\n";
        }
        return 0;
    }

    static int leave(const char *func, int line)
    {
        if(level_stack[lstack_idx] >= 4)
        {
            cout << "Leave: " << func << ": "<< line << "\n";
        }
        return 0;
    }

    static void logger(const char *func, int line, int level, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(logging_buffer, sizeof(logging_buffer), fmt, args);
        va_end(args);

        // log level 0 turns off all logging
        if(level_stack[lstack_idx] >= level)
        {
            switch(level)
            {
                case 1:
                    cout << "ERROR: ";
                    break;
                case 2:
                    cout << "WARNING: ";
                    break;
                case 3:
                    cout << "LOG: ";
                    break;
                case 4:
                    cout << "DEBUG: ";
                    break;
            }
        }
        cout << func << ": "<< line << ": " << logging_buffer << "\n";
    }

    static void push_level(int level)
    {
        if(lstack_idx < 99)
        {
            lstack_idx ++;
            level_stack[lstack_idx] = level;
        }
        // else silently fail to change the logger level
    }

    static void pop_level(void)
    {
        if(lstack_idx < 0)
            lstack_idx = 0;
        lstack_idx --;
    }

private:
    static char logging_buffer[1024];
    //static ostream log_file;
    //static int log_level;
    //static int indent_level;
    static int level_stack[100];
    static int lstack_idx;

};

#else
#   define LOG(l, s, ...)
#   define DEBUG(s, ...)
#   define WARNING(s, ...)
#   define ERROR(s, ...)
#   define ENTER()
#   define LEAVE(s, ...)
#   define RETURN(v, s, ...)    return v
#   define VOID_RET(s, ...)     return
#endif

#endif /* !_TOI_LOGGER_H_ */
