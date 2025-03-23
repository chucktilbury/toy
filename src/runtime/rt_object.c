/**
 * @file values.c
 *
 * @brief Implement the object model.
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-07
 * @version 0.0.1
 * @copyright Copyright 2025
 */
#include "runtime.h"
#include "rt_errors.h"
#include "common_memory.h"

static base_object_t* create(const char* name, const char* fname, int line, base_object_t* obj);
static void destroy(base_object_t* obj);
static base_object_t* unary_invalid_operation(base_object_t* obj);
static base_object_t* binary_invalid_operation(base_object_t* obj, base_object_t* val);

static size_t get_object_size(obj_type_t type) {

    return (type == OBJ_INT)? sizeof(rt_int_obj_t) :
        (type == OBJ_FLOAT)? sizeof(rt_float_obj_t) :
        (type == OBJ_STR)? sizeof(rt_str_obj_t) :
        (type == OBJ_BOOL)? sizeof(rt_bool_obj_t) :
        (type == OBJ_FUNC)? sizeof(rt_func_obj_t) :
        (type == OBJ_USER)? sizeof(rt_user_obj_t) : (size_t)(-1);
}

base_object_t* rt_create_object(obj_type_t type, const char* name, const char* fname, int line) {

    base_object_t* ptr = _ALLOC(get_object_size(type));
    return create(name, fname, line, ptr);
}

void rt_destroy_object(base_object_t* obj) {

    destroy(obj);
}

static base_object_t* create(const char* name, const char* fname, int line, base_object_t* ptr) {

    ptr->line_no = line;
    ptr->name = _COPY_STRING(name);
    ptr->fname = _COPY_STRING(fname);

    ptr->create = create;
    ptr->destroy = destroy;

    ptr->to_int = unary_invalid_operation;
    ptr->to_float = unary_invalid_operation;
    ptr->to_str = unary_invalid_operation;
    ptr->to_bool = unary_invalid_operation;
    ptr->to_usr = unary_invalid_operation;

    ptr->uminus_int = unary_invalid_operation;
    ptr->uminus_float = unary_invalid_operation;
    ptr->uminus_str = unary_invalid_operation;
    ptr->uminus_bool = unary_invalid_operation;
    ptr->uminus_usr = unary_invalid_operation;

    ptr->add_int = binary_invalid_operation;
    ptr->add_float = binary_invalid_operation;
    ptr->add_str = binary_invalid_operation;
    ptr->add_bool = binary_invalid_operation;
    ptr->add_usr = binary_invalid_operation;

    ptr->sub_int = binary_invalid_operation;
    ptr->sub_float = binary_invalid_operation;
    ptr->sub_str = binary_invalid_operation;
    ptr->sub_bool = binary_invalid_operation;
    ptr->sub_usr = binary_invalid_operation;

    ptr->mul_int = binary_invalid_operation;
    ptr->mul_float = binary_invalid_operation;
    ptr->mul_str = binary_invalid_operation;
    ptr->mul_bool = binary_invalid_operation;
    ptr->mul_usr = binary_invalid_operation;

    ptr->div_int = binary_invalid_operation;
    ptr->div_float = binary_invalid_operation;
    ptr->div_str = binary_invalid_operation;
    ptr->div_bool = binary_invalid_operation;
    ptr->div_usr = binary_invalid_operation;

    ptr->mod_int = binary_invalid_operation;
    ptr->mod_float = binary_invalid_operation;
    ptr->mod_str = binary_invalid_operation;
    ptr->mod_bool = binary_invalid_operation;
    ptr->mod_usr = binary_invalid_operation;

    ptr->pow_int = binary_invalid_operation;
    ptr->pow_float = binary_invalid_operation;
    ptr->pow_str = binary_invalid_operation;
    ptr->pow_bool = binary_invalid_operation;
    ptr->pow_usr = binary_invalid_operation;

    ptr->not_int = unary_invalid_operation;
    ptr->not_float = unary_invalid_operation;
    ptr->not_str = unary_invalid_operation;
    ptr->not_bool = unary_invalid_operation;
    ptr->not_usr = unary_invalid_operation;

    ptr->equ_int = binary_invalid_operation;
    ptr->equ_float = binary_invalid_operation;
    ptr->equ_str = binary_invalid_operation;
    ptr->equ_bool = binary_invalid_operation;
    ptr->equ_usr = binary_invalid_operation;

    ptr->nequ_int = binary_invalid_operation;
    ptr->nequ_float = binary_invalid_operation;
    ptr->nequ_str = binary_invalid_operation;
    ptr->nequ_bool = binary_invalid_operation;
    ptr->nequ_usr = binary_invalid_operation;

    ptr->lt_int = binary_invalid_operation;
    ptr->lt_float = binary_invalid_operation;
    ptr->lt_str = binary_invalid_operation;
    ptr->lt_bool = binary_invalid_operation;
    ptr->lt_usr = binary_invalid_operation;

    ptr->gt_int = binary_invalid_operation;
    ptr->gt_float = binary_invalid_operation;
    ptr->gt_str = binary_invalid_operation;
    ptr->gt_bool = binary_invalid_operation;
    ptr->gt_usr = binary_invalid_operation;

    ptr->lore_int = binary_invalid_operation;
    ptr->lore_float = binary_invalid_operation;
    ptr->lore_str = binary_invalid_operation;
    ptr->lore_bool = binary_invalid_operation;
    ptr->lore_usr = binary_invalid_operation;

    ptr->gore_int = binary_invalid_operation;
    ptr->gore_float = binary_invalid_operation;
    ptr->gore_str = binary_invalid_operation;
    ptr->gore_bool = binary_invalid_operation;
    ptr->gore_usr = binary_invalid_operation;

    return ptr;
}

static void destroy(base_object_t* obj) {

    RT_ASSERT(obj != NULL, "attempt to destroy a NULL object");

    if(obj->name != NULL)
        _FREE(obj->name);

    if(obj->fname != NULL)
        _FREE(obj->fname);

    _FREE(obj);
}

static base_object_t* unary_invalid_operation(base_object_t* val) {

    rt_error("invalid unary operation on %s object",
        (val->type == OBJ_INT)? "INT":
        (val->type == OBJ_FLOAT)? "FLOAT":
        (val->type == OBJ_STR)? "STR":
        (val->type == OBJ_BOOL)? "BOOL":
        (val->type == OBJ_FUNC)? "FUNC":
        (val->type == OBJ_USER)? "USER": "UNKNOWN");

}

static base_object_t* binary_invalid_operation(base_object_t* obj, base_object_t* val) {

    rt_error("invalid binary operation on %s object",
        (val->type == OBJ_INT)? "INT":
        (val->type == OBJ_FLOAT)? "FLOAT":
        (val->type == OBJ_STR)? "STR":
        (val->type == OBJ_BOOL)? "BOOL":
        (val->type == OBJ_FUNC)? "FUNC":
        (val->type == OBJ_USER)? "USER": "UNKNOWN");

}

