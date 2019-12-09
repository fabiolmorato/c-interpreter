#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

struct {
    const char* start; // holds the start to current token
    const char* current; // holds the read pointer
    unsigned    line;
} scanner;

/*
 * Initializes data above loading first position of the program
 */
void init_scanner (const char* program)
{
    scanner.start = program;
    scanner.current = program;
    if (scanner.line == 0) scanner.line = 1;
}

/*
 * Checks if character is an alphabetic character or an underscore
 */
static bool is_alpha (const char c)
{
    return (c == '_')             ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

/*
 * Checks if character is a digit.
 */
static bool is_digit (const char c)
{
    return c >= '0' && c <= '9';
}

/*
 * Checks if scanner has reached end of program.
 */
static bool scanner_end (void)
{
    return *scanner.current == '\0';
}

/*
 * Returns character and current pointer.
 */
static char peek (void)
{
    return *scanner.current;
}

/*
 * Returns the character after the current pointer.
 */
static char peek_next (void)
{
    if (scanner_end()) return '\0';
    return scanner.current[1];
}

/*
 * Sets current character to next and returns what it was.
 */
static char advance (void)
{
    scanner.current++;
    return scanner.current[-1];
}

/*
 * Checks if current character matches a expected character. If it does, advances.
 */
static bool match (const char c)
{
    if (scanner_end()) return false;
    if (peek() != c) return false;

    scanner.current++;
    return true;
}

/*
 * Creates a configured token.
 */
token_t new_token (token_type_t type)
{
    token_t token;
    
    token.type = type;
    token.lexeme = scanner.start;
    token.length = (unsigned) (scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

/*
 * Created a configured error token.
 */
token_t new_error_token (const char* error_message, unsigned line)
{
    token_t token;

    token.type = TOKEN_ERROR;
    token.lexeme = error_message;
    token.length = (unsigned) strlen(error_message);
    token.line = line;

    return token;
}

/*
 * Skips pointer to line end
 */
static void skip_to_line_end (void)
{
    while (!scanner_end() && peek() != '\n') advance();
}

/*
 * Ignores whitespace characters and comments
 */
static void skip_whitespace (void)
{
    for (;;)
    {
        char c = peek();

        switch (c)
        {
            case '\n':
                scanner.line++;
                advance();
                break;
            case ' ':
                advance();
                break;
            case '\t':
                advance();
                break;
            case '\r':
                advance();
                break;

            case '/':
                if (peek_next() == '/') skip_to_line_end();
                else return;
                break;

            default:
                return;
        }
    }
}

/*
 * Decides token type by matching expected rest of string.
 */
static token_type_t decide_keyword_type (unsigned start, unsigned length, const char* rest, token_type_t type)
{
    return memcmp(scanner.start + start, rest, length) == 0 ? type : TOKEN_IDENTIFIER;
}

/*
 * Decides token type (decides if it is a reserved word or a program identifier)
 */
static token_type_t decide_identifier_type (void)
{
    switch (scanner.start[0])
    {
        case 'a': return decide_keyword_type(1, 2, "nd", TOKEN_AND);
        case 'c': return decide_keyword_type(1, 4, "lass", TOKEN_CLASS);
        case 'e': return decide_keyword_type(1, 3, "lse", TOKEN_ELSE);
        case 'i': return decide_keyword_type(1, 1, "f", TOKEN_IF);
        case 'n': return decide_keyword_type(1, 2, "il", TOKEN_NIL);
        case 'o': return decide_keyword_type(1, 1, "r", TOKEN_OR);
        case 'p': return decide_keyword_type(1, 4, "rint", TOKEN_PRINT);
        case 'r': return decide_keyword_type(1, 5, "eturn", TOKEN_RETURN);
        case 's': return decide_keyword_type(1, 4, "uper", TOKEN_SUPER);
        case 'v': return decide_keyword_type(1, 2, "ar", TOKEN_VAR);
        case 'w': return decide_keyword_type(1, 4, "hile", TOKEN_WHILE);

        case 'f':
            switch (scanner.start[1])
            {
                case 'a': return decide_keyword_type(2, 3, "lse", TOKEN_FALSE);
                case 'o': return decide_keyword_type(2, 1, "r", TOKEN_FOR);
                case 'u': return decide_keyword_type(2, 1, "n", TOKEN_FUN);
            }
            break;
        
        case 't':
            switch (scanner.start[1])
            {
                case 'h': return decide_keyword_type(2, 2, "is", TOKEN_THIS);
                case 'r': return decide_keyword_type(2, 2, "ue", TOKEN_TRUE);
            }
            break;
    }

    return TOKEN_IDENTIFIER;
}

/*
 * Makes a token for a keyword / identifier
 */
static token_t new_keyword_token (void)
{
    while (is_alpha(peek()) || is_digit(peek())) advance();
    return new_token(decide_identifier_type());
}

/*
 * Makes a token for a string
 */
static token_t new_string_token (void)
{
    scanner.start = scanner.current;

    while (peek() != '"' && !scanner_end())
    {
        if (peek() == '\n') scanner.line++;
        if (peek() == '\\' && peek_next() == '"') advance();
        advance();
    }

    if (scanner_end()) return new_error_token("Unterminated string", scanner.line);
    // advance();

    token_t token = new_token(TOKEN_STRING);
    advance();
    return token;
}

/*
 * Makes a token for a number
 */
static token_t new_number_token (void)
{
    while (is_digit(peek())) advance();

    if (peek() == '.')
    {
        advance(); // consumes '.'
        while (is_digit(peek())) advance();
    }

    return new_token(TOKEN_NUMBER);
}

/*
 * Gets next token
 */
token_t get_next_token (void)
{
    skip_whitespace();
    scanner.start = scanner.current;

    if (scanner_end()) return new_token(TOKEN_EOF);

    const char c = advance();

    if (is_digit(c)) return new_number_token();
    if (is_alpha(c)) return new_keyword_token();

    switch (c)
    {
        case '(': return new_token(TOKEN_LEFT_PAREN);
        case ')': return new_token(TOKEN_RIGHT_PAREN);
        case '{': return new_token(TOKEN_LEFT_BRACE);
        case '}': return new_token(TOKEN_RIGHT_BRACE);
        case ',': return new_token(TOKEN_COMMA);
        case '.': return new_token(TOKEN_DOT);
        case '-': return new_token(TOKEN_MINUS);
        case '+': return new_token(TOKEN_PLUS);
        case '*': return new_token(TOKEN_STAR);
        case '/': return new_token(TOKEN_SLASH);
        case ';': return new_token(TOKEN_SEMICOLON);

        case '!': return match('=') ? new_token(TOKEN_BANG_EQUAL) : new_token(TOKEN_BANG);
        case '=': return match('=') ? new_token(TOKEN_EQUAL_EQUAL) : new_token(TOKEN_EQUAL);
        case '>': return match('=') ? new_token(TOKEN_GREATER_EQUAL) : new_token(TOKEN_GREATER);
        case '<': return match('=') ? new_token(TOKEN_LESS_EQUAL) : new_token(TOKEN_LESS);

        case '"': return new_string_token();
    }

    return new_error_token("Unexpected error.", scanner.line);
}