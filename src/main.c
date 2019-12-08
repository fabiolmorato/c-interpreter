#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner/scanner.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "utils/utils.h"

int main (void)
{
    for (;;)
    {
        printf(">> ");

        char* program = read_input();
        bool error = false;

        if (program == NULL) break;
        if (strlen(program) == 0) continue;

        token_t* tokens = tokenize(program);
        ast_t ast = parse_program(tokens, &error);

        if (error)
        {
            free(tokens);
            destroy_ast(ast);
            continue;
        }

        init_interpreter();

        value_t result = interpret_ast(ast);

        switch (result.type)
        {
            case TYPE_NUMBER:
                printf("%lf\n", *((double*) result.value));
                break;
            default:
                break;
        }

        destroy_ast(ast);
        destroy_interpreter();
        free(program);
        free(tokens);
    }
    
    return 0;
}