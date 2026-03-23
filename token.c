#include "token.h"

void print_token(int type, const char* value, int line_num)
{
    const char* name;

    switch(type)
    {
        case ENTITY: name = "ENTITY"; break;
        case MODE: name = "MODE"; break;
        case STATE: name = "STATE"; break;
        case WHEN: name = "WHEN"; break;
        case UNLESS: name = "UNLESS"; break;
        case IFNOT: name = "IFNOT"; break;
        case ELSE: name = "ELSE"; break;
        case RETURN: name = "RETURN"; break;

        case LOOP_ORBIT: name = "LOOP_ORBIT"; break;
        case LOOP_PULSE: name = "LOOP_PULSE"; break;
        case LOOP_BOUNCE: name = "LOOP_BOUNCE"; break;
        case LOOP_REPEAT: name = "LOOP_REPEAT"; break;
        case LOOP_WHILE: name = "LOOP_WHILE"; break;
        case LOOP_FOR: name = "LOOP_FOR"; break;

        case TYPE_METER: name = "TYPE_METER"; break;
        case TYPE_MOOD: name = "TYPE_MOOD"; break;
        case TYPE_ENERGY: name = "TYPE_ENERGY"; break;
        case TYPE_HEALTH: name = "TYPE_HEALTH"; break;
        case TYPE_SPEED: name = "TYPE_SPEED"; break;
        case TYPE_POWER: name = "TYPE_POWER"; break;

        case BOOL_TRUE: name = "BOOL_TRUE"; break;
        case BOOL_FALSE: name = "BOOL_FALSE"; break;

        case ARROW: name = "ARROW"; break;
        case FATARROW: name = "FATARROW"; break;
        case EQEQ: name = "EQEQ"; break;
        case NEQ: name = "NEQ"; break;
        case GTE: name = "GTE"; break;
        case LTE: name = "LTE"; break;
        case GT: name = "GT"; break;
        case LT: name = "LT"; break;

        case PLUS: name = "PLUS"; break;
        case MINUS: name = "MINUS"; break;
        case STAR: name = "STAR"; break;
        case SLASH: name = "SLASH"; break;
        case MOD: name = "MOD"; break;

        case AND: name = "AND"; break;
        case OR: name = "OR"; break;
        case NOT: name = "NOT"; break;

        case EQUALS: name = "EQUALS"; break;

        case SEMICOLON: name = "SEMICOLON"; break;
        case COMMA: name = "COMMA"; break;
        case COLON: name = "COLON"; break;
        case LPAREN: name = "LPAREN"; break;
        case RPAREN: name = "RPAREN"; break;
        case LBRACE: name = "LBRACE"; break;
        case RBRACE: name = "RBRACE"; break;
        case LBRACKET: name = "LBRACKET"; break;
        case RBRACKET: name = "RBRACKET"; break;

        case ID_LITERAL: name = "ID_LITERAL"; break;
        case STRING_LITERAL: name = "STRING_LITERAL"; break;
        case INT_LITERAL: name = "INT_LITERAL"; break;
        case FLOAT_LITERAL: name = "FLOAT_LITERAL"; break;

        case END_OF_FILE: name = "END_OF_FILE"; break;

        default: name = "UNKNOWN"; break;
    }

    if(value)
        printf("Token: %-15s Value: %-10s Line: %d\n", name, value, line_num);
    else
        printf("Token: %-15s Line: %d\n", name, line_num);
}