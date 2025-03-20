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
#include <stddef.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Objects API

// Object data types
typedef enum {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
    OBJ_BOOL,
    OBJ_FUNC,
    OBJ_USER,
} obj_type_t;

typedef struct _base_object_t {
    obj_type_t type;
    size_t line_no;     // line where this object was encountered in user source
    const char* fname;  // file name in user source
    const char* name;   // name of object for error msgs

    // state of this node
    bool is_const;  // if true the value cannot be assigned
    bool is_init;   // if false the value has never been assigned

    // default constructor and destructor
    struct _base_object_t* (*create)(const char* name, const char* fname, int line, struct _base_object_t* obj);
    void (*destroy)(struct _base_object_t* obj);

    // overloaded cast methods
    struct _base_object_t* (*to_int)(struct _base_object_t* obj);
    struct _base_object_t* (*to_float)(struct _base_object_t* obj);
    struct _base_object_t* (*to_str)(struct _base_object_t* obj);
    struct _base_object_t* (*to_bool)(struct _base_object_t* obj);
    struct _base_object_t* (*to_usr)(struct _base_object_t* obj);

    // overloaded arithmetic methods
    struct _base_object_t* (*uminus_int)(struct _base_object_t* obj);
    struct _base_object_t* (*uminus_float)(struct _base_object_t* obj);
    struct _base_object_t* (*uminus_str)(struct _base_object_t* obj);
    struct _base_object_t* (*uminus_bool)(struct _base_object_t* obj);
    struct _base_object_t* (*uminus_usr)(struct _base_object_t* obj);
    struct _base_object_t* (*add_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*add_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*add_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*add_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*add_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*sub_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*sub_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*sub_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*sub_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*sub_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mul_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mul_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mul_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mul_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mul_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*div_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*div_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*div_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*div_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*div_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mod_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mod_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mod_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mod_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*mod_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*pow_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*pow_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*pow_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*pow_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*pow_usr)(struct _base_object_t* obj, struct _base_object_t* val);

    // overloaded comparison methods, all return a BOOL object
    struct _base_object_t* (*not_int)(struct _base_object_t* obj);
    struct _base_object_t* (*not_float)(struct _base_object_t* obj);
    struct _base_object_t* (*not_str)(struct _base_object_t* obj);
    struct _base_object_t* (*not_bool)(struct _base_object_t* obj);
    struct _base_object_t* (*not_usr)(struct _base_object_t* obj);
    struct _base_object_t* (*equ_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*equ_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*equ_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*equ_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*equ_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*nequ_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*nequ_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*nequ_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*nequ_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*nequ_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lt_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lt_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lt_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lt_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lt_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gt_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gt_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gt_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gt_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gt_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lore_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lore_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lore_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lore_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*lore_usr)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gore_int)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gore_float)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gore_str)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gore_bool)(struct _base_object_t* obj, struct _base_object_t* val);
    struct _base_object_t* (*gore_usr)(struct _base_object_t* obj, struct _base_object_t* val);

} base_object_t;

#include "rt_int_obj.h"
#include "rt_float_obj.h"
#include "rt_str_obj.h"
#include "rt_bool_obj.h"
#include "rt_func_obj.h"
#include "rt_user_obj.h"

base_object_t* rt_create_object(obj_type_t type, const char* name, const char* fname, int line);
void rt_destroy_object(base_object_t* obj);

#endif /* _RT_OBJECT_H_ */

