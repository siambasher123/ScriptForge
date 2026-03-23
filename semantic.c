#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

extern int line_num;

void check_variable_declared(const char *name)
{
    if(find_symbol(name) == -1)
    {
        printf("Semantic Error at line %d: Variable '%s' used before declaration\n", line_num, name);
    }
}

void check_variable_redeclaration(const char *name)
{
    if(find_symbol(name) != -1)
    {
        printf("Semantic Error at line %d: Variable '%s' already declared\n", line_num, name);
    }
}

// New function for type checking with implicit conversion
void check_assignment_type(const char* var_name, const char* expr_type, VarType expr_vtype)
{
    VarType var_type = get_symbol_type(var_name);
    
    if(var_type == VAR_TYPE_UNKNOWN) {
        return;
    }
    
    // Implicit conversion rules
    if(var_type == expr_vtype) {
        // Same type - no conversion needed
        return;
    }
    else if(var_type == VAR_TYPE_FLOAT && expr_vtype == VAR_TYPE_INT) {
        printf("Implicit conversion: int to float for assignment to '%s'\n", var_name);
        return;
    }
    else if(var_type == VAR_TYPE_INT && expr_vtype == VAR_TYPE_FLOAT) {
        printf("Warning: Possible loss of precision when assigning float to int '%s'\n", var_name);
        return;
    }
    else if(var_type == VAR_TYPE_BOOL && is_numeric_type(expr_vtype)) {
        printf("Implicit conversion: numeric to bool for assignment to '%s'\n", var_name);
        return;
    }
    else if(is_numeric_type(var_type) && expr_vtype == VAR_TYPE_BOOL) {
        printf("Implicit conversion: bool to numeric for assignment to '%s'\n", var_name);
        return;
    }
    else {
        printf("Type Error at line %d: Cannot assign %s to %s variable '%s'\n", 
               line_num, type_to_string(expr_vtype), type_to_string(var_type), var_name);
    }
}

// New function for binary operation type checking
void check_binary_operation(const char* op, VarType left_type, VarType right_type)
{
    printf("Type checking: %s %s %s\n", 
           type_to_string(left_type), op, type_to_string(right_type));
    
    if(!check_type_compatibility(left_type, right_type, op)) {
        printf("Type Error at line %d: Incompatible types for operation '%s'\n", 
               line_num, op);
    }
}

void init_builtin_variables()
{
    printf("Initializing built-in variables...\n");
    
    if(find_symbol("see_player") == -1)
        add_symbol("see_player", "bool", 0, 1);
    
    if(find_symbol("low_health") == -1)
        add_symbol("low_health", "bool", 0, 1);
    
    if(find_symbol("tired") == -1)
        add_symbol("tired", "bool", 0, 1);
    
    printf("Built-in variables initialized.\n");
}