#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../parser/parser.h"
#include "utils.h"

static void __print_ast (ast_t root, int level)
{
    if (root == NULL) return;

    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    switch(root->type)
    {
        case NODE_PLUS:
            printf("+\n");
            break;
        case NODE_MINUS:
            printf("-\n");
            break;
        case NODE_SLASH:
            printf("/\n");
            break;
        case NODE_STAR:
            printf("*\n");
            break;
        case NODE_NUMBER:
            ;double value = *((double*) root->value);
            printf("%lf\n", value);
            break;
        default:
            printf("QPORRA!\n");
    }

    for (unsigned i = 0; i < root->children_amount; i++)
    {
        __print_ast(root->children[i], level + 1);
    }
}

void print_ast (ast_t ast)
{
    __print_ast(ast, 0);
}

token_t* tokenize (const char* program)
{
    init_scanner(program);

    token_t* tokens = calloc(16, sizeof(token_t));
    unsigned length = 0;
    unsigned size = 16;

    token_t token = get_next_token();

    for(; token.type != TOKEN_EOF; token = get_next_token())
    {
        if (length == size)
        {
            size *= 2;
            tokens = realloc(tokens, size * sizeof(token_t));
        }

        tokens[length++] = token;
    }

    tokens[length++] = token;

    return realloc(tokens, length * sizeof(token_t));
}

ast_t parse_program (token_t* tokens, bool* error)
{
    init_parser(tokens, error);
    return parse_tokens();
}

char* read_input (void)
{
    char* buffer = calloc(16, sizeof(char));
    unsigned size = 16;
    unsigned length = 0;

    char c = '\0';

    while ((c = fgetc(stdin)) != '\n')
    {
        if (c == EOF)
        {
            free(buffer);
            return NULL;
        }

        if (size == length)
        {
            size *= 2;
            buffer = realloc(buffer, size * sizeof(char));
        }

        buffer[length++] = c;
    }

    buffer[length] = '\0';

    return realloc(buffer, (length + 1) * sizeof(char));
}