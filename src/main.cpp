
#include <iostream>

#include "scanner.h"
#include "errors.h"
#include "logger.h"

using namespace std;

int ParserException::num_errors;
int ParserException::num_warnings;
int Scanner::filestack_created = 0;
FileStack* Scanner::filestack;
#ifdef LOGGING
char Logger::logging_buffer[];
int Logger::level_stack[];
int Logger::lstack_idx;
#endif

#include "token.h"

/*
 * Build string:
 * g++ -Wall -o t scanner.cpp token.cpp main.cpp filestack.cpp -DLOGGING
 */
int main(void)
{
    PUSH_LOG_LEVEL(9);
    ENTER();
    Scanner scanner = Scanner();

    try
    {
        scanner.open_file("Text-1.txt");
        while(scanner.get_token());
    }
    catch(ParserException& e)
    {
        cout << e.msg();
    }

    //delete scanner;
    RETURN(0, "no errors");
}
