/**
 * @file rt_bool_obj.h
 *
 * @brief Implement the methods for a bool intrinsic type object
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */

#ifndef _RT_BOOL_OBJ_
#define _RT_BOOL_OBJ_

#include <stddef.h>
#include <stdint.h>

#include "rt_object.h"

typedef struct _rt_bool_obj_t_ {
    base_object_t obj;
    bool value;
} rt_bool_obj_t;

#endif /* _RT_BOOL_OBJ_ */
