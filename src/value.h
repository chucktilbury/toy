/**
 * @file value.h
 * 
 * @brief Manipulate values at compile time.
 * 
 * @author Chuck Tilbury (chucktilbury@gmail.com)
 * @date 2025-03-01
 * @version 0.0.1
 * @copyright Copyright 2025
 */

#ifndef _VALUE_H_
#define _VALUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int type;
    bool is_literal;
    union {
        int64_t integer;
        double real;
        const char* str;
        bool boolean;
        void* other;
    } val;
} value_t;

value_t* create_value(int type);

#endif /* _VALUE_H_ */
