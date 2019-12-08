#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "../scanner/scanner.h"
#include "../parser/parser.h"

void     print_ast     (ast_t ast);
token_t* tokenize      (const char* program);
ast_t    parse_program (token_t* tokens, bool* error);
char*    read_input    (void);

#endif