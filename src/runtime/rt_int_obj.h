/**
 * @file rt_int_obj.h
 *
 * @brief Implement the methods for a int intrinsic type object
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#ifndef _RT_INT_OBJ_
#define _RT_INT_OBJ_

#include <stddef.h>
#include <stdint.h>

#include "rt_object.h"

typedef struct _rt_int_obj_t_ {
    base_object_t node;
    int64_t value;
} rt_int_obj_t;

#endif /* _RT_INT_OBJ_ */
