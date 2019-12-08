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

static void insert_into_loc_list (double* value)
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

static value_t get_value_at (ast_t root, unsigned position)
{
    ast_t ast = root->children[position];
    if (ast->type == NODE_NUMBER) return new_number_value(*((double*) ast->value));
    else return interpret_ast(ast);
}

static value_t sum (ast_t root)
{
    value_t left_value = get_value_at(root, 0);
    value_t right_value = get_value_at(root, 1);

    double result = *((double*) left_value.value) + *((double*) right_value.value);

    return new_number_value(result);
}

static value_t minus (ast_t root)
{
    value_t left_value = get_value_at(root, 0);
    double result = 0;
    
    if (root->children_amount == 1)
    {
        result = - *((double*) left_value.value);
    }
    else
    {
        value_t right_value = get_value_at(root, 1);
        result = *((double*) left_value.value) - *((double*) right_value.value);
    }

    return new_number_value(result);
}

static value_t times (ast_t root)
{
    value_t left_value = get_value_at(root, 0);
    value_t right_value = get_value_at(root, 1);

    double result = *((double*) left_value.value) * *((double*) right_value.value);

    return new_number_value(result);
}

static value_t divide (ast_t root)
{
    value_t left_value = get_value_at(root, 0);
    value_t right_value = get_value_at(root, 1);

    double result = *((double*) left_value.value) / *((double*) right_value.value);

    return new_number_value(result);
}

value_t interpret_ast (ast_t ast)
{
    value_t return_value = (value_t) {NULL, TYPE_NIL};

    for (unsigned i = 0; i < ast->children_amount; i++)
    {
        switch (ast->type)
        {
            case NODE_NUMBER:
                return_value = new_number_value(*((double*) ast->value));
                break;
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
            default:
                break;
        }
    }

    return return_value;
}