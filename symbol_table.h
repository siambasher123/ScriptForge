#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100

typedef enum { 
    VAR_TYPE_INT,      // Changed from TYPE_INT
    VAR_TYPE_FLOAT,    // Changed from TYPE_FLOAT
    VAR_TYPE_BOOL,     // Changed from TYPE_BOOL
    VAR_TYPE_UNKNOWN   // Changed from TYPE_UNKNOWN
} VarType;

typedef struct {
    char name[50];
    char type[20];
    VarType vtype;
    float value;
    int bool_value;
} Symbol;

extern Symbol symbol_table[MAX_SYMBOLS];
extern int symbol_count;

int add_symbol(const char *name, const char *type, float value, int is_bool);
int find_symbol(const char *name);
float get_symbol_value(const char *name);
int get_symbol_bool(const char *name);
void set_symbol_value(const char *name, float value);
void set_symbol_bool(const char *name, int val);
void print_symbol_table();

// New type conversion functions
VarType get_symbol_type(const char *name);
int is_numeric_type(VarType type);
int is_boolean_type(VarType type);
const char* type_to_string(VarType type);
float convert_to_float(VarType from_type, void* value);
int convert_to_bool(VarType from_type, void* value);
int check_type_compatibility(VarType left, VarType right, const char* op);

#endif