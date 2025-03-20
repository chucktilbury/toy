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

typedef struct _obj_node_t_ {
    obj_type_t type;
    size_t line_no;     // line where this object was encountered in user source
    const char* fname;  // file name in user source
    const char* name;   // name of object for error msgs

    // state of this node
    bool is_const;  // if true the value cannot be assigned
    bool is_init;   // if false the value has never been assigned
} obj_node_t;

// works for GCC or CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvisibility"

typedef struct _rt_int_obj_t_ {
    obj_node_t* node;

    int64_t value;

    struct _rt_int_obj_t_* (*create_from_int)(int64_t);
    struct _rt_int_float_t_* (*create_from_float)(double);
    struct _rt_int_str_t_* (*create_from_str)(const char*);
    struct _rt_int_bool_t_* (*create_from_bool)(bool);

    void (*destroy)(struct _rt_int_obj_t_*);

    struct _rt_int_obj_t_* (*to_int)(struct _rt_int_obj_t_* obj);
    struct _rt_int_float_t_* (*to_float)(struct _rt_int_obj_t_* obj);
    struct _rt_int_str_t_* (*to_str)(struct _rt_int_obj_t_* obj);
    struct _rt_int_bool_t_* (*to_bool)(struct _rt_int_obj_t_* obj);

    struct _rt_int_obj_t_* (*add_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*add_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*add_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*add_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*sub_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*sub_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*sub_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*sub_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*mul_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*mul_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*mul_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*mul_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*div_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*div_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*div_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*div_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*mod_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*mod_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*mod_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*mod_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*pow_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*pow_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*pow_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*pow_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*equ_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*equ_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*equ_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*equ_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*nequ_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*nequ_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*nequ_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*nequ_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*lt_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*lt_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*lt_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*lt_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*gt_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*gt_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*gt_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*gt_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*lore_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*lore_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*lore_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*lore_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*gore_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*gore_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*gore_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*gore_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*uminus_int)(struct _rt_int_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*uminus_float)(struct _rt_int_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*uminus_str)(struct _rt_int_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*uminus_bool)(struct _rt_int_obj_t_* obj, struct _rt_bool_obj_t_* val);

} rt_int_obj_t;

typedef struct _rt_float_obj_t_ {
    obj_node_t* node;

    double value;

    struct _rt_int_obj_t_* (*create_from_int)(int64_t);
    struct _rt_int_float_t_* (*create_from_float)(double);
    struct _rt_int_str_t_* (*create_from_str)(const char*);
    struct _rt_int_bool_t_* (*create_from_bool)(bool);

    void (*destroy)(struct _rt_float_obj_t_*);

    struct _rt_int_obj_t_* (*to_int)(struct _rt_float_obj_t_* obj);
    struct _rt_int_float_t_* (*to_float)(struct _rt_float_obj_t_* obj);
    struct _rt_int_str_t_* (*to_str)(struct _rt_float_obj_t_* obj);
    struct _rt_int_bool_t_* (*to_bool)(struct _rt_float_obj_t_* obj);

    struct _rt_int_obj_t_* (*add_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*add_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*add_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*add_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*sub_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*sub_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*sub_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*sub_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*mul_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*mul_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*mul_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*mul_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*div_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*div_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*div_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*div_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*mod_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*mod_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*mod_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*mod_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*pow_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*pow_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*pow_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*pow_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*equ_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*equ_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*equ_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*equ_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*nequ_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*nequ_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*nequ_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*nequ_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*lt_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*lt_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*lt_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*lt_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*gt_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*gt_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*gt_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*gt_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*lore_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*lore_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*lore_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*lore_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*gore_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*gore_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*gore_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*gore_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

    struct _rt_int_obj_t_* (*uminus_int)(struct _rt_float_obj_t_* obj, struct _rt_int_obj_t_* val);
    struct _rt_int_float_t_* (*uminus_float)(struct _rt_float_obj_t_* obj, struct _rt_float_obj_t_* val);
    struct _rt_int_str_t_* (*uminus_str)(struct _rt_float_obj_t_* obj, struct _rt_str_obj_t_* val);
    struct _rt_int_bool_t_* (*uminus_bool)(struct _rt_float_obj_t_* obj, struct _rt_bool_obj_t_* val);

} rt_float_obj_t;

typedef struct _rt_bool_obj_t_ {
    obj_node_t* node;

    bool value;

    struct _rt_int_obj_t_* (*create_from_int)(int64_t);
    struct _rt_int_float_t_* (*create_from_float)(double);
    struct _rt_int_str_t_* (*create_from_str)(const char*);
    struct _rt_int_bool_t_* (*create_from_bool)(bool);

    void (*destroy)(struct _rt_bool_obj_t_* obj);

    bool (*to_int)(struct _rt_bool_obj_t_* obj);
    bool (*to_float)(struct _rt_bool_obj_t_* obj);
    bool (*to_str)(struct _rt_bool_obj_t_* obj);
    bool (*to_bool)(struct _rt_bool_obj_t_* obj);

    bool (*add_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*add_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*add_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*add_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);
    bool (*sub_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*sub_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);

    bool (*sub_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*sub_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);
    bool (*mul_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*mul_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*mul_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*mul_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*div_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*div_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*div_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*div_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*mod_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*mod_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*mod_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*mod_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*pow_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*pow_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*pow_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*pow_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*equ_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*equ_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*equ_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*equ_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*nequ_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*nequ_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*nequ_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*nequ_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*lt_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*lt_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*lt_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*lt_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*gt_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*gt_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*gt_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*gt_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*lore_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*lore_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*lore_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*lore_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*gore_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*gore_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*gore_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*gore_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

    bool (*uminus_int)(struct _rt_bool_obj_t_* obj, struct _rt_int_obj_t_* val);
    bool (*uminus_float)(struct _rt_bool_obj_t_* obj, struct _rt_float_obj_t_* val);
    bool (*uminus_str)(struct _rt_bool_obj_t_* obj, struct _rt_str_obj_t_* val);
    bool (*uminus_bool)(struct _rt_bool_obj_t_* obj, struct _rt_bool_obj_t_* val);

} rt_bool_obj_t;

typedef struct _rt_str_obj_t_ {
    obj_node_t* node;

    char* buffer;
    size_t len;
    size_t cap;

    struct _rt_int_obj_t_* (*create_from_int)(int64_t);
    struct _rt_int_float_t_* (*create_from_float)(double);
    struct _rt_int_str_t_* (*create_from_str)(const char*);
    struct _rt_int_bool_t_* (*create_from_bool)(bool);

    void (*destroy)(struct _rt_str_obj_t_* obj);

    const char* (*to_int)(struct _rt_str_obj_t_* obj);
    const char* (*to_float)(struct _rt_str_obj_t_* obj);
    const char* (*to_str)(struct _rt_str_obj_t_* obj);
    const char* (*to_bool)(struct _rt_str_obj_t_* obj);

    const char* (*add_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*add_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*add_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*add_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*sub_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*sub_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*sub_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*sub_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*mul_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*mul_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*mul_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*mul_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*div_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*div_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*div_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*div_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*mod_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*mod_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*mod_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*mod_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*pow_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*pow_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*pow_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*pow_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*equ_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*equ_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*equ_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*equ_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*nequ_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*nequ_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*nequ_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*nequ_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*lt_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*lt_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*lt_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*lt_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*gt_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*gt_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*gt_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*gt_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*lore_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*lore_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*lore_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*lore_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*gore_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*gore_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*gore_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*gore_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

    const char* (*uminus_int)(struct _rt_str_obj_t_* obj, struct _rt_int_obj_t_* val);
    const char* (*uminus_float)(struct _rt_str_obj_t_* obj, struct _rt_float_obj_t_* val);
    const char* (*uminus_str)(struct _rt_str_obj_t_* obj, struct _rt_str_obj_t_* val);
    const char* (*uminus_bool)(struct _rt_str_obj_t_* obj, struct _rt_bool_obj_t_* val);

} rt_str_obj_t;

#pragma GCC diagnostic pop

#endif /* _RT_OBJECT_H_ */
