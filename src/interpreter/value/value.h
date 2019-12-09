#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum {
    TYPE_NUMBER, TYPE_BOOL,

    TYPE_NIL,
    TYPE_ERROR
} value_type_t;

typedef struct {
    union {
        double number;
        bool   boolean;
    }            value;
    value_type_t type;
} value_t;

value_t new_number_value        (double number);
value_t new_bool_value          (bool boolean);
value_t convert_value_to_number (value_t value);
value_t convert_value_to_bool   (value_t value);

#endif