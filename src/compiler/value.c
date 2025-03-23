/**
 * @file value.c
 *
 * @brief Implement values
 *
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-01
 * @version 0.0.1
 * @copyright Copyright 2025
 */

 #include "value.h"
 #include "common_memory.h"

 value_t* create_value(int type) {

    value_t* val = _ALLOC_TYPE(value_t);
    val->type = type;

    return val;
 }

