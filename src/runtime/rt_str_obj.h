/**
 * @file rt_str_obj.h
 *
 * @brief Implement the methods for a string intrinsic type object
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#ifndef _RT_STR_OBJ_H_
#define _RT_STR_OBJ_H_

#include <stddef.h>
#include <stdint.h>

#include "rt_object.h"

typedef struct _rt_str_obj_t_ {
    base_object_t obj;
    char* buffer;
    size_t len;
    size_t cap;
} rt_str_obj_t;

#endif /* _RT_STR_OBJ_H_ */
