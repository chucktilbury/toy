/*
 *  This implements a stack of files. When a file is included, the lexer
 *  switches over to that file until it ends and the pops the file off of the
 *  stack and picks up where it left off.
 *
 *  This also checks for circular dependencies. A files that has been included
 *  cannot include itself and it cannot include a file that has included it.
 *  Recursive includes are a fatal error.
 */
#ifndef _FILESTACK_H_
#define _FILESTACK_H_

#include <fstream>
#include <string>

#include "logger.h"

using namespace std;

// these are the definitions for characters are are legal or illegal to return.
typedef enum
{
    VALID_CHAR,
    INVALID_CHAR,
} _valid_char_t;

class FileStack
{
public:
    FileStack(void)
    {
        ENTER();
        top = nullptr;
        lines = 0;
        files = 0;

        for(int i = 0; i < 256; i++)
            valid_table[i] = INVALID_CHAR;
        for(int i = ' '; i <= '~'; i++)
            valid_table[i] = VALID_CHAR;
        valid_table['\r']  = VALID_CHAR;
        valid_table['\t']  = VALID_CHAR;
        valid_table['\n']  = VALID_CHAR;
        valid_table['\f']  = VALID_CHAR;

        LEAVE();
    }
    ~FileStack(void);

    bool open(const string fname);
    int get_char(void);
    void unget_char(int ch);

    const string* filename(void) { return((nullptr != top)? top->name: nullptr); }
    int lineno(void) { return((nullptr != top)? top->line: -1); }
    int total_lines(void) { return lines; }
    int total_files(void) { return files; }

private:
    void close(void);

    struct _fstack {
        string* name;
        ifstream file;
        int line;
        int ug_buffer[100];
        unsigned int ug_index;
        struct _fstack *next;
    };
    struct _fstack *top;

    int lines;
    int files;

    int valid_table[256];
};

#endif /* !_FILESTACK_H_ */
