#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"

void check_variable_declared(const char *name);
void check_variable_redeclaration(const char *name);
void init_builtin_variables(void);

// New type conversion functions
void check_assignment_type(const char* var_name, const char* expr_type, VarType expr_vtype);
void check_binary_operation(const char* op, VarType left_type, VarType right_type);

#endif