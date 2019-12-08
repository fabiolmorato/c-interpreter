#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../scanner/scanner.h"

typedef enum {
    NODE_MINUS, NODE_PLUS, NODE_SLASH, NODE_STAR,

    NODE_NUMBER,

    NODE_ERROR, NODE_ROOT,
    NODE_EOF
} ast_node_type_t;

typedef struct __ast_node {
    ast_node_type_t     type;
    void*               value;
    unsigned            length;
    struct __ast_node** children;
    unsigned            children_amount; 
} ast_node_t; 

typedef ast_node_t* ast_t;

void  init_parser  (token_t* tokens, bool* error);
ast_t parse_tokens (void);
void  destroy_ast  (ast_t ast);

#endif