/**
 * @file rt_bool_obj.c
 *
 * @brief Implement the methods for a boolean object
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-20
 * @version 0.0.1
 * @copyright Copyright 2025
 */

#include "rt_object.h"
#include "rt_errors.h"
#include "common_memory.h"

static base_object_t* create(const char* name, const char* fname, int line, base_object_t* obj);
static void destroy(base_object_t* obj);
static base_object_t* to_int(base_object_t* obj);
static base_object_t* to_float(base_object_t* obj);
static base_object_t* to_str(base_object_t* obj);
static base_object_t* to_bool(base_object_t* obj);
static base_object_t* to_usr(base_object_t* obj);
static base_object_t* uminus(base_object_t* obj);
static base_object_t* not(base_object_t* obj);
static base_object_t* add(base_object_t* obj, base_object_t* val);
static base_object_t* sub(base_object_t* obj, base_object_t* val);
static base_object_t* mul(base_object_t* obj, base_object_t* val);
static base_object_t* divide(base_object_t* obj, base_object_t* val);
static base_object_t* mod(base_object_t* obj, base_object_t* val);
static base_object_t* pow(base_object_t* obj, base_object_t* val);
static base_object_t* equ(base_object_t* obj, base_object_t* val);
static base_object_t* nequ(base_object_t* obj, base_object_t* val);
static base_object_t* lt(base_object_t* obj, base_object_t* val);
static base_object_t* gt(base_object_t* obj, base_object_t* val);
static base_object_t* lore(base_object_t* obj, base_object_t* val);
static base_object_t* gore(base_object_t* obj, base_object_t* val);

static rt_bool_obj_t* create_object(const char* name, const char* fname, int line) {

    rt_bool_obj_t* ptr = _ALLOC_TYPE(rt_bool_obj_t);

    ptr->obj.type = OBJ_BOOL;
    ptr->obj.line_no = line;
    ptr->obj.name = _COPY_STRING(name);
    ptr->obj.fname = _COPY_STRING(fname);

    ptr->obj.create = create;
    ptr->obj.destroy = destroy;

    ptr->obj.to_int = to_int;
    ptr->obj.to_float = to_float;
    ptr->obj.to_str = to_str;
    ptr->obj.to_bool = to_bool;
    ptr->obj.to_usr = to_bool;

    ptr->obj.uminus_int = uminus;
    ptr->obj.uminus_float = uminus;
    ptr->obj.uminus_str = uminus;
    ptr->obj.uminus_bool = uminus;
    ptr->obj.uminus_usr = uminus;

    ptr->obj.add_int = add;
    ptr->obj.add_float = add;
    ptr->obj.add_str = add;
    ptr->obj.add_bool = add;
    ptr->obj.add_usr = add;

    ptr->obj.sub_int = sub;
    ptr->obj.sub_float = sub;
    ptr->obj.sub_str = sub;
    ptr->obj.sub_bool = sub;
    ptr->obj.sub_usr = sub;

    ptr->obj.mul_int = mul;
    ptr->obj.mul_float = mul;
    ptr->obj.mul_str = mul;
    ptr->obj.mul_bool = mul;
    ptr->obj.mul_usr = mul;

    ptr->obj.div_int = divide;
    ptr->obj.div_float = divide;
    ptr->obj.div_str = divide;
    ptr->obj.div_bool = divide;
    ptr->obj.div_usr = divide;

    ptr->obj.mod_int = mod;
    ptr->obj.mod_float = mod;
    ptr->obj.mod_str = mod;
    ptr->obj.mod_bool = mod;
    ptr->obj.mod_usr = mod;

    ptr->obj.pow_int = pow;
    ptr->obj.pow_float = pow;
    ptr->obj.pow_str = pow;
    ptr->obj.pow_bool = pow;
    ptr->obj.pow_usr = pow;

    ptr->obj.equ_int = equ;
    ptr->obj.equ_float = equ;
    ptr->obj.equ_str = equ;
    ptr->obj.equ_bool = equ;
    ptr->obj.equ_usr = equ;

    ptr->obj.nequ_int = nequ;
    ptr->obj.nequ_float = nequ;
    ptr->obj.nequ_str = nequ;
    ptr->obj.nequ_bool = nequ;
    ptr->obj.nequ_usr = nequ;

    ptr->obj.lt_int = lt;
    ptr->obj.lt_float = lt;
    ptr->obj.lt_str = lt;
    ptr->obj.lt_bool = lt;
    ptr->obj.lt_usr = lt;

    ptr->obj.gt_int = gt;
    ptr->obj.gt_float = gt;
    ptr->obj.gt_str = gt;
    ptr->obj.gt_bool = gt;
    ptr->obj.gt_usr = gt;

    ptr->obj.lore_int = lore;
    ptr->obj.lore_float = lore;
    ptr->obj.lore_str = lore;
    ptr->obj.lore_bool = lore;
    ptr->obj.lore_usr = lore;

    ptr->obj.gore_int = gore;
    ptr->obj.gore_float = gore;
    ptr->obj.gore_str = gore;
    ptr->obj.gore_bool = gore;
    ptr->obj.gore_usr = gore;

    return ptr;
}

static base_object_t* create(const char* name, const char* fname, int line, base_object_t* obj) {

    rt_bool_obj_t* ptr;

    if(obj != NULL) {
        switch(obj->type) {
            case OBJ_BOOL:
                ptr = create_object(name, fname, line);
                ptr->value = ((rt_bool_obj_t*)obj)->value;
                break;
            case OBJ_FLOAT:
                ptr = create_object(name, fname, line);
                // WARNING: unreliable comparison
                ptr->value = (((rt_bool_obj_t*)obj)->value == 0.0)? false: true;
                break;
            case OBJ_INT:
                ptr = create_object(name, fname, line);
                ptr->value = (((rt_bool_obj_t*)obj)->value == 0)? false: true;
                break;
            case OBJ_FUNC:
            case OBJ_STR:
                ptr = create_object(name, fname, line);
                // always true
                ptr->value = true;
                break;
            case OBJ_USER:
                ptr = create_object(name, fname, line);
                ptr->value = ptr->obj.to_bool((base_object_t*)ptr);
                break;
            default:
                rt_error("Unknown object type: %d", obj->type);
        }
    }
    else {
        ptr = create_object(name, fname, line);
        ptr->value = false;
    }

    return (base_object_t*)obj;
}

static void destroy(base_object_t* obj) {

    if(obj != NULL) {
        _FREE(obj->name);
        _FREE(obj->fname);
        _FREE(obj);
    }
}

static base_object_t* to_int(base_object_t* obj) {

    RT_ASSERT(obj != NULL, "casting bool to int");
    RT_ASSERT(obj->type == OBJ_BOOL, "casting bool to int");

    rt_bool_obj_t* ptr = (rt_bool_obj_t*)obj;
    rt_int_obj_t* retv = (rt_int_obj_t*)rt_create_object(OBJ_INT, NULL, NULL, 0);
    retv->value = (int64_t)ptr->value;

    return retv;
}

static base_object_t* to_float(base_object_t* obj) {

    RT_ASSERT(obj != NULL, "casting bool to float");
    RT_ASSERT(obj->type == OBJ_BOOL, "casting bool to float");

    rt_bool_obj_t* ptr = (rt_bool_obj_t*)obj;
    rt_float_obj_t* retv = (rt_float_obj_t*)rt_create_object(OBJ_FLOAT, NULL, NULL, 0);
    retv->value = (double)ptr->value;

    return retv;
}

static base_object_t* to_str(base_object_t* obj) {

    RT_ASSERT(obj != NULL, "casting bool to string");
    RT_ASSERT(obj->type == OBJ_BOOL, "casting bool to string");

    rt_bool_obj_t* ptr = (rt_bool_obj_t*)obj;
    rt_str_obj_t* retv = (rt_str_obj_t*)rt_create_object(OBJ_STR, NULL, NULL, 0);
    //retv->value = (int64_t)ptr->value;

    return retv;
}

// guarantee a copy of the object
static base_object_t* to_bool(base_object_t* obj) {

    RT_ASSERT(obj != NULL, "casting bool to bool");
    RT_ASSERT(obj->type == OBJ_BOOL, "casting bool to bool");

    rt_bool_obj_t* ptr = (rt_bool_obj_t*)obj;
    rt_bool_obj_t* retv = (rt_bool_obj_t*)rt_create_object(OBJ_BOOL, NULL, NULL, 0);
    retv->value = ptr->value;

    return retv;
}

static base_object_t* to_usr(base_object_t* obj) {

}

static base_object_t* uminus(base_object_t* obj) {

}

static base_object_t* not(base_object_t* obj) {

}

static base_object_t* add(base_object_t* obj, base_object_t* val) {

}

static base_object_t* sub(base_object_t* obj, base_object_t* val) {

}

static base_object_t* mul(base_object_t* obj, base_object_t* val) {

}

static base_object_t* divide(base_object_t* obj, base_object_t* val) {

}

static base_object_t* mod(base_object_t* obj, base_object_t* val) {

}

static base_object_t* pow(base_object_t* obj, base_object_t* val) {

}

static base_object_t* equ(base_object_t* obj, base_object_t* val) {

}

static base_object_t* nequ(base_object_t* obj, base_object_t* val) {

}

static base_object_t* lt(base_object_t* obj, base_object_t* val) {

}

static base_object_t* gt(base_object_t* obj, base_object_t* val) {

}

static base_object_t* lore(base_object_t* obj, base_object_t* val) {

}

static base_object_t* gore(base_object_t* obj, base_object_t* val) {

}

