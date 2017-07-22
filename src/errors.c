
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errors.h"
#include "scanner.h"

// globals that count errors.
static int errors = 0;
static int warnings = 0;
static int fatal_errors = 0;
static FILE *outs= NULL;

#ifdef DEBUGGING
static int debug_level = -1; // -1 = all debugging info shown
static char indent_level = 0;

void set_debug_level(int level)
{
    debug_level = level;
}

int get_debug_level(int level)
{
    return debug_level;
}

/*
 *  level is a number 0 - 9 that indicates whether a message should be shown.
 *  A level of 0 indicates that it should be shown when the minimum level is
 *  enabled. A higher debug level number indicates fewer messages. A lower level
 *  to this function indicates a greater liklyhood that the message will be
 *  shown.
 */
int show_debug_msg(const char* func, int line, int level, char *fmt, ...)
{
    va_list args;
    int i;

    if(level > debug_level)
    {
        fprintf(outs, "DEBUG: ");
        for(i = 0; i < indent_level; i++)
            fprintf(outs, " ");
        fprintf(outs, "%s: %d: ",func, line);
        va_start(args, fmt);
        vfprintf(outs, fmt, args);
        va_end(args);
        fprintf(outs, "\n");
    }

    return 0;
}

int show_enter_msg(const char* func)
{
    int i;

    if(9 > debug_level)
    {
        fprintf(outs, "ENTER: ");
        for(i = 0; i < indent_level; i++)
            fprintf(outs, " ");
        fprintf(outs, "%s\n",func);

        indent_level++;
    }
    return 0;
}

int show_leave_msg(const char* func, int line, char *fmt, ...)
{
    va_list args;
    int i;

    if(9 > debug_level)
    {
        indent_level--;
        fprintf(outs, "LEAVE: ");
        for(i = 0; i < indent_level; i++)
            fprintf(outs, " ");
        fprintf(outs, "%s: %d: ",func, line);
        va_start(args, fmt);
        vfprintf(outs, fmt, args);
        va_end(args);
        fprintf(outs, "\n");
    }

    return 0;
}

#endif

void init_errors(FILE *stream)
{
    errors = 0;
    warnings = 0;
    if(stream == NULL)
        outs = stderr;
    else
        outs = stream;
}

void show_info_msg(char *fmt, ...)
{
    va_list args;

    fprintf(outs, "INFO: ");
    va_start(args, fmt);
    vfprintf(outs, fmt, args);
    va_end(args);
    fprintf(outs, "\n");
}

void show_syntax_error(char *fmt, ...)
{
    va_list args;

    errors++;
    fprintf(outs, "Syntax error: %s: %d: ", get_file_name(), get_line_number());
    va_start(args, fmt);
    vfprintf(outs, fmt, args);
    va_end(args);
    fprintf(outs, "\n");
}

void show_syntax_warning(char *fmt, ...)
{
    va_list args;

    warnings++;
    fprintf(outs, "Syntax error: %s: %d: ", get_file_name(), get_line_number());
    va_start(args, fmt);
    vfprintf(outs, fmt, args);
    va_end(args);
    fprintf(outs, "\n");
}

void fatal_error(char *fmt, ...)
{
    va_list args;

    fatal_errors++;
    fprintf(outs, "Fatal error: %s: %d: ", get_file_name(), get_line_number());
    va_start(args, fmt);
    vfprintf(outs, fmt, args);
    va_end(args);
    fprintf(outs, "\n");

    exit(1);
}

int get_num_warnings(void)
{
    return warnings;
}

int get_num_errors(void)
{
    return errors;
}

void show_result(void)
{
    printf("errors: %d, warnings: %d\n", errors, warnings);
}
