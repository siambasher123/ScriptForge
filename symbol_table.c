#include <stdio.h>
#include <string.h>
#include <math.h>
#include "symbol_table.h"

Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

int find_symbol(const char *name)
{
    for(int i = 0; i < symbol_count; i++)
    {
        if(strcmp(symbol_table[i].name, name) == 0)
            return i;
    }
    return -1;
}

VarType get_symbol_type(const char *name)
{
    int idx = find_symbol(name);
    if(idx == -1) {
        return VAR_TYPE_UNKNOWN;
    }
    return symbol_table[idx].vtype;
}

int is_numeric_type(VarType type)
{
    return (type == VAR_TYPE_INT || type == VAR_TYPE_FLOAT);
}

int is_boolean_type(VarType type)
{
    return (type == VAR_TYPE_BOOL);
}

const char* type_to_string(VarType type)
{
    switch(type) {
        case VAR_TYPE_INT: return "int";
        case VAR_TYPE_FLOAT: return "float";
        case VAR_TYPE_BOOL: return "bool";
        default: return "unknown";
    }
}

float convert_to_float(VarType from_type, void* value)
{
    switch(from_type) {
        case VAR_TYPE_INT:
            return (float)(*(int*)value);
        case VAR_TYPE_FLOAT:
            return *(float*)value;
        case VAR_TYPE_BOOL:
            return (*(int*)value) ? 1.0f : 0.0f;
        default:
            return 0.0f;
    }
}

int convert_to_bool(VarType from_type, void* value)
{
    switch(from_type) {
        case VAR_TYPE_INT:
            return (*(int*)value) != 0;
        case VAR_TYPE_FLOAT:
            return fabs(*(float*)value) > 0.0001f;
        case VAR_TYPE_BOOL:
            return *(int*)value;
        default:
            return 0;
    }
}

int check_type_compatibility(VarType left, VarType right, const char* op)
{
    // Both numeric types - compatible
    if(is_numeric_type(left) && is_numeric_type(right)) {
        return 1;
    }
    
    // Both boolean types - compatible for comparison
    if(is_boolean_type(left) && is_boolean_type(right)) {
        // Boolean operations are allowed
        if(strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
           strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
            return 1;
        }
        // Arithmetic operations on booleans - warn but allow
        printf("Warning: Boolean operation '%s' on boolean values\n", op);
        return 1;
    }
    
    // Mixed numeric and boolean - allow with conversion
    if((is_numeric_type(left) && is_boolean_type(right)) ||
       (is_boolean_type(left) && is_numeric_type(right))) {
        printf("Implicit conversion: %s to numeric for operation '%s'\n", 
               type_to_string(right), op);
        return 1;
    }
    
    printf("Type error: Cannot perform '%s' between %s and %s\n", 
           op, type_to_string(left), type_to_string(right));
    return 0;
}

int add_symbol(const char *name, const char *type, float value, int is_bool)
{
    if(symbol_count >= MAX_SYMBOLS) {
        printf("Semantic Error: Symbol table full\n");
        return 0;
    }
    if(find_symbol(name) != -1) {
        printf("Semantic Error: Variable '%s' redeclared\n", name);
        return 0;
    }

    strcpy(symbol_table[symbol_count].name, name);
    strcpy(symbol_table[symbol_count].type, type);

    if(is_bool) {
        symbol_table[symbol_count].vtype = VAR_TYPE_BOOL;
        symbol_table[symbol_count].bool_value = (int)value;
        symbol_table[symbol_count].value = 0;
    } else {
        // Check if it's an integer literal (no decimal point)
        if(strchr(type, '.') == NULL && strcmp(type, "int") == 0) {
            symbol_table[symbol_count].vtype = VAR_TYPE_INT;
        } else {
            symbol_table[symbol_count].vtype = VAR_TYPE_FLOAT;
        }
        symbol_table[symbol_count].value = value;
        symbol_table[symbol_count].bool_value = 0;
    }

    symbol_count++;
    return 1;
}

float get_symbol_value(const char *name)
{
    int idx = find_symbol(name);
    if(idx == -1) {
        printf("Semantic Error: Variable '%s' not declared\n", name);
        return 0;
    }
    return symbol_table[idx].value;
}

int get_symbol_bool(const char *name)
{
    int idx = find_symbol(name);
    if(idx == -1) {
        printf("Semantic Error: Variable '%s' not declared\n", name);
        return 0;
    }
    return symbol_table[idx].bool_value;
}

void set_symbol_value(const char *name, float value)
{
    int idx = find_symbol(name);
    if(idx == -1) {
        printf("Semantic Error: Variable '%s' not declared\n", name);
        return;
    }
    symbol_table[idx].value = value;
}

void set_symbol_bool(const char *name, int val)
{
    int idx = find_symbol(name);
    if(idx == -1) {
        printf("Semantic Error: Variable '%s' not declared\n", name);
        return;
    }
    symbol_table[idx].bool_value = val;
}

void print_symbol_table()
{
    printf("\n=== Symbol Table ===\n");
    for(int i = 0; i < symbol_count; i++)
    {
        if(symbol_table[i].vtype == VAR_TYPE_BOOL)
            printf("%s (%s) = %d\n", symbol_table[i].name, symbol_table[i].type, symbol_table[i].bool_value);
        else if(symbol_table[i].vtype == VAR_TYPE_INT)
            printf("%s (%s) = %.0f\n", symbol_table[i].name, symbol_table[i].type, symbol_table[i].value);
        else
            printf("%s (%s) = %.2f\n", symbol_table[i].name, symbol_table[i].type, symbol_table[i].value);
    }
    printf("====================\n");
}