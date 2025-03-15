/**
 * @file strings.c
 *
 * @brief Implement the strings.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-07
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include <stddef.h>
#include "runtime.h"

//-----------------------------------------------------------------------------
// strings API

/**
 * @brief Return the formated string with garbage collections in place.
 *
 * @param fmt
 * @param ...
 * @return const char*
 */
runtime_string_t* runtime_string_format(const char* fmt, ...) {

    runtime_string_t* ptr = NULL;

    return ptr;
}

/**
 * @brief Send the string to stdout.
 *
 * @param str
 */
void runtime_string_print(runtime_string_t* str) {

}

