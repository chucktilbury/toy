

#include "runtime.h"

int rt_error_number = 0;

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
 * @param name
 * @param line
 */
void rt_enter_func(const char* name, int line) {

}

/**
 * @brief Call this to pop the call stack and collect the garbage.
 *
 */
void rt_leave_func(void) {

}
