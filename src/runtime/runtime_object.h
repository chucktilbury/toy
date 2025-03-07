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
#ifndef _RUNTIME_OBJECT_H_
#define _RUNTIME_OBJECT_H_

//-----------------------------------------------------------------------------
// Objects API
// Object data types
typedef enum {
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
    OBJ_BOOL,
    OBJ_OTHER,
} object_type_t;

// Object data structure
typedef struct _object_t_ {

} runtime_object_t;


#endif /* _RUNTIME_OBJECT_H_ */
