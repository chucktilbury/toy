/*
 *  This implements the exceptions that are internal to the parser.
 */

#ifndef _ERRORS_H_
#define _ERRORS_H_

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

class EndOfInput: public exception, public ParserException
{
public:
    EndOfInput(void)
    {
        sprintf((char*)message_buffer, "end of input");
    }
};

class ScannerError: public exception, public ParserException
{
public:
    ScannerError(const string fmt, ...)
    {
        num_errors++;

        sprintf((char*)message_buffer, "Scanner error: ");
        char* ptr = (char*)&message_buffer[strlen(message_buffer)];

        va_list args;
        va_start(args, fmt);
        vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
        va_end(args);
    }
};

class InternalScannerError: public exception, public ParserException
{
public:
    InternalScannerError(const string fmt, ...)
    {
        num_errors++;

        sprintf((char*)message_buffer, "Internal scanner error: ");
        char* ptr = (char*)&message_buffer[strlen(message_buffer)];

        va_list args;
        va_start(args, fmt);
        vsnprintf(ptr, sizeof(message_buffer)-strlen(message_buffer), fmt.c_str(), args);
        va_end(args);
    }
};

class InvalidCharacterError: public exception, public ParserException
{
public:
    InvalidCharacterError(int ch)
    {
        num_errors++;

        sprintf((char*)message_buffer, "Invalid character in input: 0x%02X", ch);
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

// TODO: connect this up to the scanner for line number and file name
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
