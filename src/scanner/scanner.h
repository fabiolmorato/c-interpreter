#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS, TOKEN_SEMICOLON, TOKEN_SLASH,
    TOKEN_STAR,

    TOKEN_BANG, TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER,
    TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FUN, TOKEN_FOR, TOKEN_IF,
    TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, TOKEN_TRUE,
    TOKEN_VAR, TOKEN_WHILE,

    TOKEN_ERROR,
    TOKEN_EOF
} token_type_t;

typedef struct {
    token_type_t type;
    const char*  lexeme;
    unsigned     line;
    unsigned     length;
} token_t;

void    init_scanner    (const char* data);
token_t get_next_token  (void);
token_t new_error_token (const char* error_message, unsigned line);
token_t new_token       (token_type_t type);

#endif