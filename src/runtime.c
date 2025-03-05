

#include "runtime.h"

int runtime_error_number = 0;

/**
 * @brief Initialize all of the runtime data structures, including the
 * command line interface.
 *
 * @param argc
 * @param argv
 * @param arge
 */
void runtime_main_init(int argc, char** argv, char** arge) {

}

/**
 * @brief Destroy all of the runtime data ahead of the operating system.
 *
 */
void runtime_main_uninit(void) {

}

//-----------------------------------------------------------------------------
// strings API

/**
 * @brief Return the formated string with garbage collections in place.
 *
 * @param fmt
 * @param ...
 * @return const char*
 */
char* runtime_string_format(const char* fmt, ...) {

}

//-----------------------------------------------------------------------------
// garbage collection API

/**
 * @brief Call this before using any GC_* functions. Creates internal data
 * structures, which are global to the program. There is only one heap.
 *
 */
void runtime_gc_init(void) {

}

/**
 * @brief Optional function to call at the end of the program. Normally all
 * of the allocated memory will be cleaned up by the operating system, but
 * this does it manually.
 *
 */
void runtime_gc_uninit(void) {

}

/**
 * @brief Call this as the first thing in a function. Assign the return value
 * to a local and then use that when calling GC_end().
 *
 * @return runtime_gc_node_t*
 */
runtime_gc_node_t* runtime_gc_begin(void) {

}

/**
 * @brief Call this just before a return(); The first parameter is the
 * allocation context. The second parameter is the intended return value, that
 * will be passed to the return operator. If there is no allocated memory that
 * will be returned, then the second parameter is NULL. For example when
 * returning something like an INT.
 *
 * @param node
 * @param ptr
 */
void runtime_gc_end(runtime_gc_node_t* node, void* ptr) {

}

/**
 * @brief Allocate a root node. This always clears memory and there is no
 * calloc() equivalent call. That is simulated with macros.
 *
 * @param size
 * @return void*
 */
void* runtime_gc_alloc(unsigned long size) {

}

/**
 * @brief Reallocate a node. can be either root or child and status is not
 * changed.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* runtime_gc_realloc(void* ptr, unsigned long size) {

}

/**
 * @brief Allocate a child node. i.e. the first parameter is a data structure
 * (aka root) and the returned value is referred to by it.
 *
 * @param ptr
 * @param size
 * @return void*
 */
void* runtime_gc_alloc_child(void* ptr, unsigned long size) {

}

/**
 * @brief Explicitly free an allocation and all of its children. Normally not
 * required or used.
 *
 * @param ptr
 */
void runtime_gc_free(void* ptr) {

}
