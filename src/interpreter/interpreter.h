#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../parser/parser.h"

typedef enum {
    TYPE_NUMBER, TYPE_BOOL, TYPE_NAN,

    TYPE_NIL,
    TYPE_ERROR
} value_type_t;

typedef struct {
    void*        value;
    value_type_t type;
} value_t;

void    init_interpreter    (void);
void    destroy_interpreter (void);
value_t interpret_ast       (ast_t ast);

#endif