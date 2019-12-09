#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../parser/parser.h"

void    init_interpreter    (void);
void    destroy_interpreter (void);
value_t interpret_ast       (ast_t ast);

#endif