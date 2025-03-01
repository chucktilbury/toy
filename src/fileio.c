
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

#define USE_TRACE
#include "trace.h"

static const char* base_file_name = NULL;
static string_list_t* toy_env = NULL;
static char buffer[PATH_MAX]; // returning a pointer to this

/*
 * Handle errors around realpath().
 */
static inline const char* get_path(const char* str) {

    if(NULL == realpath(str, buffer))
        FATAL("cannot resolve path: \"%s\": %s", str, strerror(errno));

    return buffer;
}

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

static bool file_exists(const char* fname) {

    struct stat sb;
    return ((stat(fname, &sb) == 0));
}

static void setup_env(void) {

    toy_env = create_string_list();

    add_env("TOY_PATH");
    add_dirs("..");
    add_env("PATH");
}

const char* find_file(const char* fname) {

    ENTER;

    char* found = NULL;

    TRACE("searching for \"%s\"", fname);

    if(toy_env == NULL)
        setup_env();

    int mark = 0;
    string_buffer_t* s;

    while(NULL != (s = iterate_string_list(toy_env, &mark))) {
        strncpy(buffer, raw_string_buffer(s), PATH_MAX);
        strcat(buffer, "/");
        strcat(buffer, fname);

        TRACE("try: %s", buffer);
        if(file_exists(buffer)) {
            TRACE("found: %s", buffer);
            found = _COPY_STRING((buffer));
            break;
        }
    }

    if(found == NULL)
        RETURN(fname);
    else
        RETURN(found);
}

const char* get_base_file_name(void) {

    return base_file_name;
}

void set_base_file_name(const char* fname) {

    base_file_name = fname;
}
