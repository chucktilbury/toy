/**
 * @file fileio.c
 * @brief Handle operating system specific file operations.
 * 
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-02-28
 * @version 0.0.1
 * @copyright Copyright 2025
 */
// all functions defined in scanner.l
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "string_list.h"
#include "memory.h"
#include "errors.h"

//#define USE_TRACE
#include "trace.h"

static const char* base_file_name = NULL;
static string_list_t* toy_env = NULL;
static char buffer[PATH_MAX]; // returning a pointer to this

/**
 * @brief Handle errors around realpath().
 * 
 * @param str 
 * @return const char* 
 */
static inline const char* get_path(const char* str) {

    if(NULL == realpath(str, buffer))
        FATAL("cannot resolve path: \"%s\": %s", str, strerror(errno));

    return buffer;
}

/**
 * @brief Add a path defined in the environment to the internal finder path.
 * 
 * @param str 
 */
static void add_env(const char* str) {

    if(str != NULL) {
        char* tmp = getenv(str);
        if(tmp != NULL) {
            tmp = _COPY_STRING(tmp);
            //printf("%s = %s\n", str, tmp);
            if(tmp != NULL) {
                char* s;
                char* save;
                char* f = ":";
                s = strtok(tmp, f);
                while(s != NULL) {
                    add_string_list(toy_env, create_string_buffer(s));
                    s = strtok(NULL, f);
                }

                _FREE(tmp);
            }
        }
    }
}

/**
 * @brief Add a directory and all sub directories to the internal finder path.
 * 
 * @param dname 
 */
static void add_dirs(const char* dname) {

    char* tmp = NULL;

    tmp = (char*)get_path(dname);
    strcat(tmp, "/*");
    glob_t gstruct;
    glob(tmp, GLOB_ONLYDIR, NULL, &gstruct);

    //printf("paths: %lu\n", gstruct.gl_pathc);
    for(int i = 0; i < gstruct.gl_pathc; i++) {
        //printf("%d. %s\n", i+1, gstruct.gl_pathv[i]);
        add_pointer_list(toy_env, create_string_buffer(gstruct.gl_pathv[i]));
    }
}

/**
 * @brief See if the file exists.
 * 
 * @param fname 
 * @return true 
 * @return false 
 */
static bool file_exists(const char* fname) {

    struct stat sb;
    return ((stat(fname, &sb) == 0));
}

/**
 * @brief Create the internal finder path environment.
 * 
 */
static void setup_env(void) {

    toy_env = create_string_list();

    add_env("TOY_PATH");
    add_dirs("..");
    add_env("PATH");
}

/**
 * @brief Find a file. Returns the full path given just the name.
 * 
 * @param fname 
 * @return const char* 
 */
const char* find_file(const char* fname) {

    ENTER;

    char* found = NULL;

    // add the ".toy" on the end if it was not specified
    char* tmp_name = strrchr(fname, '.');
    if(NULL == tmp_name || strcmp(tmp_name, ".toy")) {
        tmp_name = _ALLOC(PATH_MAX);
        strcpy(tmp_name, fname);
        strcat(tmp_name, ".toy");
    }
    else
        tmp_name = _COPY_STRING(fname);

    TRACE("searching for \"%s\"", tmp_name);

    if(toy_env == NULL)
        setup_env();

    int mark = 0;
    string_buffer_t* s;

    while(NULL != (s = iterate_string_list(toy_env, &mark))) {
        strncpy(buffer, raw_string_buffer(s), PATH_MAX);
        strcat(buffer, "/");
        strcat(buffer, tmp_name);

        TRACE("try: %s", buffer);
        if(file_exists(buffer)) {
            TRACE("found: %s", buffer);
            found = _COPY_STRING((buffer));
            break;
        }
    }

    _FREE(tmp_name);

    if(found == NULL)
        RETURN(fname);
    else
        RETURN(found);
}

/**
 * @brief Get the base file name object
 * 
 * @return const char* 
 */
const char* get_base_file_name(void) {

    return base_file_name;
}

/**
 * @brief Set the base file name object
 * 
 * @param fname 
 */
void set_base_file_name(const char* fname) {

    base_file_name = fname;
}
