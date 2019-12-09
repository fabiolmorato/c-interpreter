#include <stdio.h>
#include <stdlib.h>
#include "../parser/parser.h"
#include "value/value.h"
#include "interpreter.h"

static value_t sum (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    double result = 0;
    
    if (root->children_amount == 1)
    {
        result = + left_value.value.number;
    }
    else
    {
        value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));
        result = left_value.value.number + right_value.value.number;
    }

    return new_number_value(result);
}

static value_t minus (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    double result = 0;
    
    if (root->children_amount == 1)
    {
        result = - left_value.value.number;
    }
    else
    {
        value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));
        result = left_value.value.number - right_value.value.number;
    }

    return new_number_value(result);
}

static value_t times (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));

    double result = left_value.value.number * right_value.value.number;

    return new_number_value(result);
}

static value_t divide (ast_t root)
{
    value_t left_value = convert_value_to_number(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_number(interpret_ast(root->children[1]));

    double result = left_value.value.number / right_value.value.number;

    return new_number_value(result);
}

static value_t negate (ast_t root)
{
    value_t value = convert_value_to_bool(interpret_ast(root->children[0]));
    bool bool_value = value.value.boolean;
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

        result = left_value.value.number == right_value.value.number;
    }
    else if (left_value.type == TYPE_BOOL || right_value.type == TYPE_BOOL)
    {
        if (left_value.type != TYPE_BOOL) left_value = convert_value_to_bool(left_value);
        else right_value = convert_value_to_bool(right_value);

        result = left_value.value.boolean == right_value.value.boolean;
    }
    else
    {
        result = left_value.type == right_value.type;
    }

    return new_bool_value(result);
}

static value_t not_equal (ast_t root)
{
    return new_bool_value(equal(root).value.boolean);
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
        result = left_value.value.number < right_value.value.number;
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
        result = left_value.value.number <= right_value.value.number;
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
        result = left_value.value.number > right_value.value.number;
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
        result = left_value.value.number >= right_value.value.number;
    }

    return new_bool_value(result);
}

static value_t and (ast_t root)
{
    value_t left_value = convert_value_to_bool(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_bool(interpret_ast(root->children[1]));

    bool result = left_value.value.boolean && right_value.value.boolean;

    return new_bool_value(result);
}

static value_t or (ast_t root)
{
    value_t left_value = convert_value_to_bool(interpret_ast(root->children[0]));
    value_t right_value = convert_value_to_bool(interpret_ast(root->children[1]));

    bool result = left_value.value.boolean || right_value.value.boolean;

    return new_bool_value(result);
}

value_t interpret_ast (ast_t ast)
{
    value_t return_value = (value_t) {{0}, TYPE_NIL};

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