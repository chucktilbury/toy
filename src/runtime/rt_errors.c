/**
 * @file rt_errors.c
 *
 * @brief Runtime errors API
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-19
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


 void rt_dump_callstack(void) {

   fprintf(stderr, "\n-------------------\nCall Stack\n-------------------\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "\n-------------------\n");
 }

 void rt_error(const char* fmt, ...) {

   va_list args;

   fprintf(stderr, "runtime error: ");
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);

   rt_dump_callstack();

   exit(1);
}

