#include <stdio.h>
#include <stdlib.h>
#include "../parser/parser.h"
#include "interpreter.h"

void**   loc_list = NULL;
unsigned loc_list_size = 16;
unsigned loc_list_length = 0;

void init_interpreter (void)
{
    destroy_interpreter();
    loc_list = calloc(loc_list_size, sizeof(double*));
}

void destroy_interpreter (void)
{
    if (loc_list != NULL)
    {
        for (unsigned i = 0; i < loc_list_length; i++) free(loc_list[i]);
        free(loc_list);
        loc_list_length = 0;
        loc_list_size = 16;
        loc_list = NULL;
    }
}

static void insert_into_loc_list (void* value)
{
    if (loc_list_size == loc_list_length)
    {
        loc_list_size *= 2;
        loc_list = realloc(loc_list, loc_list_size * sizeof(double*));
    }

    loc_list[loc_list_length++] = value;
}

static value_t new_number_value (double number)
{
    value_t value;
    double* loc_val = calloc(1, sizeof(double));

    *loc_val = number;
    value.value = loc_val;
    value.type = TYPE_NUMBER;

    insert_into_loc_list(loc_val);

    return value;
}

static value_t new_bool_value (bool boolean)
{
    value_t value;
    bool* loc_val = calloc(1, sizeof(bool));

    *loc_val = boolean;
    value.value = loc_val;
    value.type = TYPE_BOOL;

    insert_into_loc_list(loc_val);

    return value;
}

static value_t convert_value_to_number (value_t value)
{
    value_t return_value;
    return_value.type = TYPE_NUMBER;

    switch (value.type)
    {
        case TYPE_NUMBER:
            return_value.value = value.value;
            break;
        case TYPE_BOOL:
            ;double* bool_to_number = calloc(1, sizeof(double));
            insert_into_loc_list(bool_to_number);
            *bool_to_number = *((bool*) value.value) ? 1 : 0;
            return_value.value = bool_to_number;
            break;
        default:
            return_value.type = TYPE_NAN;
            return_value.value = NULL;
    }

    return return_value;
}

static value_t convert_value_to_bool (value_t value)
{
    value_t return_value;
    return_value.type = TYPE_BOOL;

    switch (value.type)
    {
        case TYPE_NUMBER:
            ;bool* number_to_bool = calloc(1, sizeof(bool));
            insert_into_loc_list(number_to_bool);
            *number_to_bool = *((double*) value.value) != 0; 
            return_value.value = number_to_bool;
            break;
        case TYPE_BOOL:
            return_value.value = value.value;
            break;
        case TYPE_NIL:
            ;bool* nil_to_bool = calloc(1, sizeof(bool));
            insert_into_loc_list(nil_to_bool);
            *nil_to_bool = false;
            return_value.value = nil_to_bool;
            break;
        default:
            return_value.type = TYPE_ERROR;
            return_value.value = NULL;
    }

    return return_value;
}

static value_t sum (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    double result = 0;
    
    if (root->children_amount == 1)
    {
        result = + *((double*) left_value.value);
    }
    else
    {
        value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));
        result = *((double*) left_value.value) + *((double*) right_value.value);
    }

    return new_number_value(result);
}

static value_t minus (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    double result = 0;
    
    if (root->children_amount == 1)
    {
        result = - *((double*) left_value.value);
    }
    else
    {
        value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));
        result = *((double*) left_value.value) - *((double*) right_value.value);
    }

    return new_number_value(result);
}

static value_t times (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));

    double result = *((double*) left_value.value) * *((double*) right_value.value);

    return new_number_value(result);
}

static value_t divide (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));

    double result = *((double*) left_value.value) / *((double*) right_value.value);

    return new_number_value(result);
}

static value_t negate (ast_t root)
{
    value_t value = convert_value_to_bool(interpret_ast(root->children[0]));
    bool bool_value = *((bool*) value.value);
    return new_bool_value(!bool_value);
}

static value_t equal (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type == TYPE_NUMBER || right_value.type == TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);

        result = *((double*) left_value.value) == *((double*) right_value.value);
    }
    else if (left_value.type == TYPE_BOOL || right_value.type == TYPE_BOOL)
    {
        if (left_value.type != TYPE_BOOL) left_value = convert_value_to_bool(left_value);
        else right_value = convert_value_to_bool(right_value);

        result = *((bool*) left_value.value) == *((bool*) right_value.value);
    }
    else
    {
        result = left_value.type == right_value.type;
    }

    return new_bool_value(result);
}

static value_t not_equal (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type == TYPE_NUMBER || right_value.type == TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);

        result = *((double*) left_value.value) != *((double*) right_value.value);
    }
    else if (left_value.type == TYPE_BOOL || right_value.type == TYPE_BOOL)
    {
        if (left_value.type != TYPE_BOOL) left_value = convert_value_to_bool(left_value);
        else right_value = convert_value_to_bool(right_value);

        result = *((bool*) left_value.value) != *((bool*) right_value.value);
    }
    else
    {
        result = left_value.type != right_value.type;
    }

    return new_bool_value(result);
}

static value_t less (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type != TYPE_NUMBER || right_value.type != TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);
    }

    if (left_value.type == TYPE_NUMBER && right_value.type == TYPE_NUMBER)
    {
        result = *((double*) left_value.value) < *((double*) right_value.value);
    }

    return new_bool_value(result);
}

static value_t less_equal (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type != TYPE_NUMBER || right_value.type != TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);
    }

    if (left_value.type == TYPE_NUMBER && right_value.type == TYPE_NUMBER)
    {
        result = *((double*) left_value.value) <= *((double*) right_value.value);
    }

    return new_bool_value(result);
}

static value_t greater (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type != TYPE_NUMBER || right_value.type != TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);
    }

    if (left_value.type == TYPE_NUMBER && right_value.type == TYPE_NUMBER)
    {
        result = *((double*) left_value.value) > *((double*) right_value.value);
    }

    return new_bool_value(result);
}

static value_t greater_equal (ast_t root)
{
    value_t left_value = interpret_ast(root->children[0]);
    value_t right_value = interpret_ast(root->children[1]);

    bool result = false;

    if (left_value.type != TYPE_NUMBER || right_value.type != TYPE_NUMBER)
    {
        if (left_value.type != TYPE_NUMBER) left_value = convert_value_to_number(left_value);
        else right_value = convert_value_to_number(right_value);
    }

    if (left_value.type == TYPE_NUMBER && right_value.type == TYPE_NUMBER)
    {
        result = *((double*) left_value.value) >= *((double*) right_value.value);
    }

    return new_bool_value(result);
}

static value_t and (ast_t root)
{
    value_t left_value = convert_value_to_bool(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_bool(interpret_ast(root->children[1]));

    bool result = *((bool*) left_value.value) && *((bool*) right_value.value);

    return new_bool_value(result);
}

static value_t or (ast_t root)
{
    value_t left_value = convert_value_to_bool(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_bool(interpret_ast(root->children[1]));

    bool result = *((bool*) left_value.value) || *((bool*) right_value.value);

    return new_bool_value(result);
}

value_t interpret_ast (ast_t ast)
{
    value_t return_value = (value_t) {NULL, TYPE_NIL};

    switch (ast->type)
    {
        case NODE_NUMBER:
            return_value = new_number_value(*((double*) ast->value));
            break;
        case NODE_BOOL:
            return_value = new_bool_value(*((bool*) ast->value));
            break;
        default:
            for (unsigned i = 0; i < ast->children_amount; i++)
            {
                if (ast->type == NODE_ROOT)
                {
                    return_value = interpret_ast(ast->children[i]);
                }
                else
                {
                    switch (ast->type)
                    {
                        case NODE_PLUS:
                            return_value = sum(ast);
                            break;
                        case NODE_MINUS:
                            return_value = minus(ast);
                            break;
                        case NODE_SLASH:
                            return_value = divide(ast);
                            break;
                        case NODE_STAR:
                            return_value = times(ast);
                            break;
                        case NODE_BANG:
                            return_value = negate(ast);
                            break;
                        case NODE_EQUAL_EQUAL:
                            return_value = equal(ast);
                            break;
                        case NODE_LESS:
                            return_value = less(ast);
                            break;
                        case NODE_LESS_EQUAL:
                            return_value = less_equal(ast);
                            break;
                        case NODE_GREATER:
                            return_value = greater(ast);
                            break;
                        case NODE_GREATER_EQUAL:
                            return_value = greater_equal(ast);
                            break;
                        case NODE_BANG_EQUAL:
                            return_value = not_equal(ast);
                            break;
                        case NODE_AND:
                            return_value = and(ast);
                            break;
                        case NODE_OR:
                            return_value = or(ast);
                            break;
                        default:
                            break;
                    }
                }
            }
    }

    return return_value;
}