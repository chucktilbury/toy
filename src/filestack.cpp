
#include <iostream>
#include <fstream>

#include "filestack.h"
#include "errors.h"
#include "logger.h"

using namespace std;

FileStack::~FileStack(void)
{
    ENTER();
    if(top != nullptr)
    {
        struct _fstack *next, *ptr;
        for(ptr = top; ptr != nullptr; ptr = next)
        {
            next = ptr->next;
            delete ptr->name;
            ptr->file.close();
            delete ptr;
            cout << "delete file stack entry\n";
        }
    }
    LEAVE();
}

bool FileStack::open(string fname)
{
    struct _fstack *ptr = new struct _fstack;

    ptr->name = new string(fname);
    ptr->file = ifstream(fname);
    if(ptr->file.is_open())
    {
        //cout << &ptr->file << "\n";
        ptr->line = 0;
        ptr->ug_index = 0;
        ptr->next = nullptr;

        if(top != nullptr)
        {
            ptr->next = top;
        }
        top = ptr;
        files ++;
        return true;
    }
    else
    {
        throw FileError(fname, strerror(errno));
        return false;
    }
}

void FileStack::close(void)
{
    struct _fstack *ptr = top;
    ENTER();

    if(ptr != nullptr)
    {
        top = ptr->next;
        delete ptr->name;
        if(ptr->file.is_open())
            ptr->file.close();
        delete ptr;
    }
    // else fail silently
    LEAVE();
}

int FileStack::get_char(void)
{

    int ch = -1;

    if(top != nullptr)
    {
        if(top->ug_index != 0)
        {
            // return from the unget buffer
            // take care not to unget an invalid character
            ch = top->ug_buffer[top->ug_index];
            top->ug_index--;
        }
        else
        {
            ch = top->file.get();
        }

        if(ch == EOF)
        {
            close();
            return get_char(); // enter recursively
        }
        else if(valid_table[ch] == VALID_CHAR)
        {
            switch(ch)
            {
                // The token generator ignores white space
                case '\n':
                    top->line++;
                    lines++;
                    break; // needed to scan comments
                case ' ':
                case '\t':
                case '\r':
                    ch = ' ';
                    break;
            }
        }
        else
        {
            throw InvalidCharacterError(ch);
        }
    }
    else
    {
        throw EndOfInput();
    }

    return ch;
}

void FileStack::unget_char(int ch)
{
    ENTER();
    if(top != nullptr)
    {
        if(top->ug_index+1 < sizeof(top->ug_buffer)/sizeof(top->ug_buffer[0]))
        {
            top->ug_index++; // 0 indicates the empty buffer.
            top->ug_buffer[top->ug_index] = ch;
        }
        // else fail silently. This should never happen, since the lexer never
        // ungets more than one character.
    }
    // else fail silently. The next read will be EOI anyhow.
    LEAVE();
}
