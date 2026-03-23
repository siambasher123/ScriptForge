#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum { 
    NODE_ENTITY, 
    NODE_MODE, 
    NODE_DECL, 
    NODE_WHEN, 
    NODE_UNLESS, 
    NODE_IFNOT, 
    NODE_IF,
    NODE_ELSE,
    NODE_PRINT,
    NODE_LOOP, 
    NODE_ASSIGN,
    NODE_PROGRAM,
    NODE_EXPR_VAL,
    NODE_EXPR_VAR,
    NODE_EXPR_OP,
    NODE_CONDITION,
    NODE_CONDITION_CMP,
    NODE_LIST
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char* name;
    char* value;
    int loop_count;
    struct ASTNode* children;
    struct ASTNode* next;
} ASTNode;

// Core AST creation functions
ASTNode* create_program(ASTNode* entities);
ASTNode* create_entity(const char* name, ASTNode* children);
ASTNode* create_mode(const char* name, ASTNode* children);
ASTNode* create_decl(const char* type, const char* name, const char* value);
ASTNode* create_transition(NodeType type, const char* condition, const char* action);
ASTNode* create_loop(int count, ASTNode* children);
ASTNode* create_assignment(const char* var, const char* value);

// Expression and condition functions
ASTNode* create_expr_val(const char* val);
ASTNode* create_expr_var(const char* name);
ASTNode* create_expr_op(const char* op, ASTNode* left, ASTNode* right);
ASTNode* create_condition(const char* expr);
ASTNode* create_condition_cmp(const char* var, const char* op, const char* val);

// Helper functions
ASTNode* append_node(ASTNode* list, ASTNode* node);
char* expr_to_string(ASTNode* expr);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

// Code generation
void generate_csharp(ASTNode* root, FILE* out);
void generate_ir(ASTNode* node, FILE* out);  // Intermediate code generation
void optimize_ir(ASTNode* node);              // Code optimization

#endif