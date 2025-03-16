/**
 * @file object.h
 *
 * @brief runtime objects.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-07
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#ifndef _RT_OBJECT_H_
#define _RT_OBJECT_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Objects API

// Object data types
typedef enum {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
    OBJ_BOOL,
    OBJ_INT_LIT,
    OBJ_FLOAT_LIT,
    OBJ_STR_LIT,
    OBJ_BOOL_LIT,
    OBJ_FUNC,
    OBJ_USER,
} object_type_t;

// Object data structure
typedef struct _object_t_ {
    object_type_t type;
    union {
        uint64_t integer;
        double real;
        rt_string_t* string;
        bool boolean;
        void* other;
    } v;
    struct _object_t_* attributes;
} rt_object_t;


#endif /* _RT_OBJECT_H_ */
