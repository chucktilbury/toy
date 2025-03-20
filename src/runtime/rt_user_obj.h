/**
 * @file rt_usr_obj.h
 *
 * @brief User defined object
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#ifndef _RT_USER_OBJ_H_
#define _RT_USER_OBJ_H_

#include "rt_object.h"

// generic and fake user object with no attributes or methods.
typedef struct _rt_user_obj_t_ {
    base_object_t obj;
} rt_user_obj_t;

#endif /* _RT_USER_OBJ_H_ */


