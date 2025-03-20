/**
 * @file runtime.c
 *
 * @brief Implement the runtime functions that are not really a good fit
 * for another module.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */

#include "runtime.h"


/**
 * @brief Initialize all of the runtime data structures, including the
 * command line interface.
 *
 * @param argc
 * @param argv
 * @param arge
 */
void rt_main_init(int argc, char** argv, char** arge) {

}

/**
 * @brief Destroy all of the runtime data ahead of the operating system.
 *
 */
void rt_main_uninit(void) {

}

/**
 * @brief Call this function to set up GC and call stack for a function.
 *
 * @param name -- The proto type name
 * @param line -- Line number where the function is defined
 */
void rt_enter_func(const char* name, const char* fname, int line) {

}

/**
 * @brief Call this to pop the call stack and collect the garbage.
 *
 */
void rt_leave_func(void) {

}
