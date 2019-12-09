#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "value.h"

value_t new_number_value (double number)
{
    value_t value;

    value.value.number = number;
    value.type = TYPE_NUMBER;

    return value;
}

value_t new_bool_value (bool boolean)
{
    value_t value;
    
    value.value.boolean = boolean;
    value.type = TYPE_BOOL;

    return value;
}

value_t convert_value_to_number (value_t value)
{
    value_t return_value;
    return_value.type = TYPE_NUMBER;

    switch (value.type)
    {
        case TYPE_NUMBER:
            return_value.value.number = value.value.number;
            break;
        case TYPE_BOOL:
            return_value.value.number = value.value.boolean ? 1 : 0;
            break;
        case TYPE_NIL:
            return_value.value.number = 0;
            break;
        default:
            return_value.value.number = NAN;
    }

    return return_value;
}

value_t convert_value_to_bool (value_t value)
{
    value_t return_value;
    return_value.type = TYPE_BOOL;

    switch (value.type)
    {
        case TYPE_NUMBER:
            return_value.value.boolean = value.value.number != 0;
            break;
        case TYPE_BOOL:
            return_value.value.boolean = value.value.boolean;
            break;
        case TYPE_NIL:
            return_value.value.boolean = false;
            break;
        default:
            return_value.type = TYPE_ERROR;
            return_value.value.number = 0;
    }

    return return_value;
}