
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "errors.h"

/*
 *  This is a simple wrapper for the flex scanner, which is written in C.
 */
extern "C" {
    int _open_file(char *fname);
    int _get_line_number(void);
    char *_get_file_name(void);
    int _get_token(void);
    const char *_get_tok_str(void);
    int _get_total_lines(void);
};


class Scanner {

    public:
    void open_file(const string fname)
    {
        int retv = _open_file((char*)fname.c_str());
        if(retv != 0)
        {
            switch(retv)
            {
                case -1:
                    throw MemoryError("for file allocation");
                case -2:
                    throw MemoryError("for file name allocation");
                case -3:
                    throw FileError(fname, string(strerror(errno)));
                default:
                    throw ParserException("unknown error");
            }
        }
    }

    int get_line_number(void)
    {
        return _get_line_number();
    }

    const string get_file_name(void)
    {
        return string(_get_file_name());
    }

    int get_token(void)
    {
        int retv = _get_token();
        if(retv == 0)
            throw EndOfInput();

        return retv;
    }

    const string get_tok_str(void)
    {
        return string(_get_tok_str());
    }

    int get_total_lines(void)
    {
        return _get_total_lines();
    }
};

#endif /* _SCANNER_H_ */
