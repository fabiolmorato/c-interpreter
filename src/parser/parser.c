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

// static token_t peek_next (void)
// {
//     if (parser_end()) return peek();
//     return parser.tokens[parser.current + 1];
// }

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

static bool expect (unsigned size, ...)
{
    va_list args;
    va_start(args, size);

    for (unsigned i =0; i < size; i++)
    {
        token_type_t type = va_arg(args, token_type_t);
        if (check_token_type(type))
        {
            va_end(args);
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
            double* loc_double = calloc(1, sizeof(double));
            *loc_double = value;
            ast->value = (void*) loc_double;
            break;

        case TOKEN_AND:
            ast->type = NODE_AND;
            ast->value = NULL;
            break;
        case TOKEN_OR:
            ast->type = NODE_OR;
            ast->value = NULL;
            break;
        case TOKEN_BANG_EQUAL:
            ast->type = NODE_BANG_EQUAL;
            ast->value = NULL;
            break;
        case TOKEN_BANG:
            ast->type = NODE_BANG;
            ast->value = NULL;
            break;
        case TOKEN_EQUAL_EQUAL:
            ast->type = NODE_EQUAL_EQUAL;
            ast->value = NULL;
            break;
        case TOKEN_GREATER:
            ast->type = NODE_GREATER;
            ast->value = NULL;
            break;
        case TOKEN_GREATER_EQUAL:
            ast->type = NODE_GREATER_EQUAL;
            ast->value = NULL;
            break;
        case TOKEN_LESS:
            ast->type = NODE_LESS;
            ast->value = NULL;
            break;
        case TOKEN_LESS_EQUAL:
            ast->type = NODE_LESS_EQUAL;
            ast->value = NULL;
            break;
        case TOKEN_TRUE:
            ast->type = NODE_BOOL;
            bool* loc_true = calloc(1, sizeof(bool));
            *loc_true = true;
            ast->value = (void*) loc_true;
            break;
        case TOKEN_FALSE:
            ast->type = NODE_BOOL;
            bool* loc_false = calloc(1, sizeof(bool));
            *loc_false = false;
            ast->value = (void*) loc_false;
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
            if (token.type == TOKEN_IDENTIFIER) printf("Token '%.5s' could not be understood.\n", token.lexeme);
            else printf("Token type %d not yet implemented.\n", token.type);
    }

    return ast;
}

static ast_t new_root_ast (void)
{
    ast_t ast = calloc(1, sizeof(ast_node_t));

    ast->children = calloc(1, sizeof(ast_node_t));
    ast->children_amount = 0;
    ast->type = NODE_ROOT;
    ast->value = NULL;

    return ast;
}

static ast_t add_child (ast_t root, ast_t child)
{
    if (child == NULL) return root;
    
    root->children = realloc(root->children, ++root->children_amount * sizeof(ast_t));
    root->children[root->children_amount - 1] = child;

    return root;
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
        if (child != NULL) root->children[i] = child;
        else root->children_amount--;
    }

    va_end(args);
    return root;
}

static ast_t new_unary_operation_subtree (ast_t root, ast_t new_child)
{
    ast_t subtree = root;
    while (subtree->children_amount == 1) subtree = subtree->children[0];

    subtree->children = realloc(subtree->children, sizeof(ast_t));
    subtree->children_amount = 1;
    subtree->children[0] = new_child;

    return root;
}

static ast_t logic_expression (void);

static ast_t primary (void)
{
    if (match(1, TOKEN_LEFT_PAREN))
    {
        token_t left_paren = peek_previous();

        ast_t ast = logic_expression();
        if (match(1, TOKEN_RIGHT_PAREN)) return ast;

        destroy_ast(ast);
        return new_ast(new_error_token("Expected closing parenthesis", left_paren.line));
    }

    if (match(3, TOKEN_NUMBER, TOKEN_TRUE, TOKEN_FALSE))
    {
        token_t previous = peek_previous();
        return new_ast(previous);
    }
    
    char error[2048];
    char lexeme[1024];
    
    for (unsigned i = 0; i < 1024; i++) error[i] = '\0', lexeme[i] = '\0';
    for (unsigned i = 1024; i < 2048; i++) error[i] = '\0';
    memcpy(lexeme, peek().lexeme, peek().length);

    sprintf(error, "Expected to find a value but found '%s'", lexeme);
    return new_ast(new_error_token(error, peek().line));
}

static ast_t unary (void)
{
    ast_t expr = NULL;

    while (expect(3, TOKEN_MINUS, TOKEN_BANG, TOKEN_PLUS))
    {
        token_t token = advance();
        if (expr == NULL) expr = new_ast(token);
        else expr = new_unary_operation_subtree(expr, new_ast(token));
    }

    if (expr == NULL) return primary();

    expr = new_unary_operation_subtree(expr, primary());
    return expr;
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

static ast_t comparison (void)
{
    ast_t expr = addition();

    while (match(4, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL))
    {
        token_t previous = peek_previous();
        ast_t right = addition();
        expr = new_operation_subtree(previous, 2, expr, right);
    }

    return expr;
}

static ast_t equality (void)
{
    ast_t expr = comparison();

    while (match(2, TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL))
    {
        token_t previous = peek_previous();
        ast_t right = comparison();
        expr = new_operation_subtree(previous, 2, expr, right);
    }

    return expr;
}

static ast_t expression (void)
{
    ast_t root = new_root_ast();
    add_child(root, equality());

    while (match(1, TOKEN_COMMA))
    {
        add_child(root, equality());
    }

    return root;
}

static ast_t logic_expression (void)
{
    ast_t expr = expression();

    while (match(2, TOKEN_AND, TOKEN_OR))
    {
        token_t previous = peek_previous();
        ast_t right = expression();
        expr = new_operation_subtree(previous, 2, expr, right);
    }

    return expr;
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
    ast_t root = logic_expression();
    return root;
}