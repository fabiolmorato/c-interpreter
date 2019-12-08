#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "parser.h"
#include "../scanner/scanner.h"

struct {
    token_t* tokens;
    unsigned current;
    bool*    error;
} parser;

void init_parser (token_t* tokens, bool* error)
{
    parser.tokens = tokens;
    parser.current = 0;
    parser.error = error;
}

static void set_error (void)
{
    *parser.error = true;
}

static token_t peek (void)
{
    return parser.tokens[parser.current];
}

static bool parser_end (void)
{
    return peek().type == TOKEN_EOF;
}

static token_t peek_previous (void)
{
    if (parser.current == 0) return peek();
    return parser.tokens[parser.current - 1];
}

static token_t advance (void)
{
    if (parser_end()) return peek();
    parser.current++;
    return peek_previous();
}

static bool check_token_type (token_type_t type)
{
    if (parser_end()) return false;
    return peek().type == type;
}

static bool match (unsigned size, ...)
{
    va_list args;
    va_start(args, size);

    for (unsigned i =0; i < size; i++)
    {
        token_type_t type = va_arg(args, token_type_t);
        if (check_token_type(type))
        {
            va_end(args);
            advance();
            return true;
        }
    }

    va_end(args);
    return false;
}

static ast_t new_ast (const token_t token)
{
    ast_t ast = calloc(1, sizeof(ast_node_t));

    ast->children = calloc(1, sizeof(ast_node_t));
    ast->children_amount = 0;

    switch (token.type)
    {
        case TOKEN_PLUS:
            ast->type = NODE_PLUS;
            ast->value = NULL;
            break;
        case TOKEN_MINUS:
            ast->type = NODE_MINUS;
            ast->value = NULL;
            break;
        case TOKEN_STAR:
            ast->type = NODE_STAR;
            ast->value = NULL;
            break;
        case TOKEN_SLASH:
            ast->type = NODE_SLASH;
            ast->value = NULL;
            break;
        case TOKEN_NUMBER:
            ast->type = NODE_NUMBER;
            double value = 0;
            char lexeme[129];
            memcpy(lexeme, token.lexeme, token.length);
            lexeme[token.length] = '\0';
            sscanf(lexeme, "%lf", &value);
            double* loc_val = calloc(1, sizeof(double));
            *loc_val = value;
            ast->value = (void*) loc_val;
            break;
        case TOKEN_ERROR:
            set_error();
            if (token.line) printf("Error at line %u: %s\n", token.line, token.lexeme);
            else printf("Error: %s\n", token.lexeme);
            break;
        case TOKEN_EOF:
            set_error();
            printf("Error: Unexpectedly reached EOF at line %u.\n", token.line);
            break;
        default:
            printf("Token type %d not yet implemented.\n", token.type);
    }

    return ast;
}

static ast_t new_operation_subtree (token_t token, unsigned children_amount, ...)
{
    va_list args;
    va_start(args, children_amount);

    ast_t root = new_ast(token);
    root->children = realloc(root->children, children_amount * sizeof(ast_t));
    root->children_amount = children_amount;

    for (unsigned i = 0; i < children_amount; i++)
    {
        ast_t child = va_arg(args, ast_t);
        root->children[i] = child;
    }

    va_end(args);
    return root;
}

static ast_t expression (void);

static ast_t primary (void)
{
    if (match(1, TOKEN_LEFT_PAREN))
    {
        token_t left_paren = peek_previous();

        ast_t ast = expression();
        if (match(1, TOKEN_RIGHT_PAREN)) return ast;

        destroy_ast(ast);
        return new_ast(new_error_token("Expected closing parenthesis", left_paren.line));
    }

    return new_ast(advance());
}

static ast_t unary (void)
{
    if (match(1, TOKEN_MINUS))
    {
        token_t previous = peek_previous();
        ast_t right = primary();
        return new_operation_subtree(previous, 1, right);
    }

    return primary();
}

static ast_t multiplication (void)
{
    ast_t expr = unary();

    while (match(2, TOKEN_STAR, TOKEN_SLASH))
    {
        token_t previous = peek_previous();
        ast_t right = unary();
        expr = new_operation_subtree(previous, 2, expr, right);
    }

    return expr;
}

static ast_t addition (void)
{
    ast_t expr = multiplication();

    while (match(2, TOKEN_PLUS, TOKEN_MINUS))
    {
        token_t previous = peek_previous();
        ast_t right = multiplication();
        expr = new_operation_subtree(previous, 2, expr, right);
    }

    return expr;
}

static ast_t expression (void)
{
    return addition();
}

void destroy_ast (ast_t root)
{
    for (unsigned i = 0; i < root->children_amount; i++)
    {
        destroy_ast(root->children[i]);
    }
    
    if (root->value != NULL) free(root->value);
    free(root->children);
    free(root);
}

ast_t parse_tokens (void)
{
    ast_t root = expression();
    return root;
}