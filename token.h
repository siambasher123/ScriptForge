#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

/* -------------------------
   Token enumeration
------------------------- */
typedef enum TokenType {
    // Keywords
    ENTITY, MODE, STATE, WHEN, UNLESS, IFNOT, ELSE, RETURN,

    // Loops
    LOOP_ORBIT, LOOP_PULSE, LOOP_BOUNCE, LOOP_REPEAT, LOOP_WHILE, LOOP_FOR,

    // Types
    TYPE_METER, TYPE_MOOD, TYPE_ENERGY, TYPE_HEALTH, TYPE_SPEED, TYPE_POWER,

    // Boolean literals
    BOOL_TRUE, BOOL_FALSE,

    // Operators
    ARROW, FATARROW, EQEQ, NEQ, GTE, LTE, GT, LT,
    PLUS, MINUS, STAR, SLASH, MOD,
    AND, OR, NOT,
    EQUALS,

    // Punctuation
    SEMICOLON, COMMA, COLON,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,

    // Literals
    ID_LITERAL, STRING_LITERAL, INT_LITERAL, FLOAT_LITERAL,

    // Special token for EOF
    END_OF_FILE
} TokenType;

/* -------------------------
   Optional: print token helper
------------------------- */
void print_token(int type, const char* value, int line_num);

#endif