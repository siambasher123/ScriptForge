#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static int temp_counter = 0;
static int label_counter = 0;
static int loop_counter = 0;

static char* new_temp() {
    char* temp = malloc(32);
    sprintf(temp, "t%d", temp_counter++);
    return temp;
}

static char* new_label() {
    char* label = malloc(32);
    sprintf(label, "L%d", label_counter++);
    return label;
}

// Helper to generate IR for a statement list (linked list)
static void generate_ir_list(ASTNode* node, FILE* out) {
    ASTNode* current = node;
    while (current) {
        generate_ir(current, out);
        current = current->next;
    }
}

void generate_ir(ASTNode* node, FILE* out) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            fprintf(out, "; Three-Address Code (Intermediate Representation)\n");
            fprintf(out, "; ============================================\n\n");
            // Process all entities in program
            generate_ir_list(node->children, out);
            break;
            
        case NODE_ENTITY:
            fprintf(out, "; Entity: %s\n", node->name ? node->name : "unknown");
            // First process declarations
            ASTNode* child = node->children;
            while (child && child->type == NODE_DECL) {
                generate_ir(child, out);
                child = child->next;
            }
            // Then process modes
            while (child) {
                if (child->type == NODE_MODE) {
                    generate_ir(child, out);
                }
                child = child->next;
            }
            break;
            
        case NODE_MODE:
            fprintf(out, "\n; Mode: %s\n", node->name ? node->name : "unknown");
            fprintf(out, "label_%s:\n", node->name);
            // Process all statements in mode body (linked list)
            generate_ir_list(node->children, out);
            break;
            
        case NODE_DECL:
            fprintf(out, "DECL %s = %s\n", node->name, node->value);
            break;
            
        case NODE_ASSIGN:
            // Output assignment - node->value contains the expression as string
            fprintf(out, "%s = %s\n", node->name, node->value);
            break;
            
        case NODE_PRINT:
            // Print statement - node->name contains the expression/string
            fprintf(out, "print %s\n", node->name);
            break;
            
        case NODE_WHEN:
            // when condition -> target
            if (node->name && node->value) {
                fprintf(out, "if %s goto %s\n", node->name, node->value);
            }
            break;
            
        case NODE_UNLESS:
            // unless condition -> target (if !condition)
            if (node->name && node->value) {
                fprintf(out, "if !(%s) goto %s\n", node->name, node->value);
            }
            break;
            
        case NODE_IFNOT:
            // ifnot condition -> target (if !condition)
            if (node->name && node->value) {
                fprintf(out, "if !(%s) goto %s\n", node->name, node->value);
            }
            break;
            
        case NODE_IF: {
            char* else_label = new_label();
            char* end_label = new_label();
            
            // Generate condition check
            if (node->name) {
                fprintf(out, "if %s == 0 goto %s\n", node->name, else_label);
            }
            
            // If block body
            generate_ir_list(node->children, out);
            fprintf(out, "goto %s\n", end_label);
            
            // Else block (if exists)
            fprintf(out, "%s:\n", else_label);
            if (node->next && node->next->type == NODE_ELSE) {
                generate_ir_list(node->next->children, out);
            }
            
            fprintf(out, "%s:\n", end_label);
            
            free(else_label);
            free(end_label);
            break;
        }
            
        case NODE_LOOP: {
            char* start_label = new_label();
            char* end_label = new_label();
            char* counter_var = malloc(32);
            sprintf(counter_var, "loop_cnt_%d", loop_counter++);
            
            // Initialize counter
            fprintf(out, "%s = %d\n", counter_var, node->loop_count);
            
            // Loop start
            fprintf(out, "%s:\n", start_label);
            fprintf(out, "if %s <= 0 goto %s\n", counter_var, end_label);
            
            // Loop body
            generate_ir_list(node->children, out);
            
            // Decrement counter and loop back
            fprintf(out, "%s = %s - 1\n", counter_var, counter_var);
            fprintf(out, "goto %s\n", start_label);
            
            // Loop end
            fprintf(out, "%s:\n", end_label);
            
            free(start_label);
            free(end_label);
            free(counter_var);
            break;
        }
            
        case NODE_EXPR_VAL:
            fprintf(out, "%s", node->value);
            break;
            
        case NODE_EXPR_VAR:
            fprintf(out, "%s", node->name);
            break;
            
        case NODE_EXPR_OP: {
            char* temp = new_temp();
            fprintf(out, "%s = ", temp);
            
            // Left operand
            if (node->children) {
                generate_ir(node->children, out);
            }
            
            // Operator
            fprintf(out, " %s ", node->name);
            
            // Right operand
            if (node->next) {
                generate_ir(node->next, out);
            }
            
            fprintf(out, "\n");
            free(temp);
            break;
        }
            
        case NODE_CONDITION:
            if (node->name) {
                fprintf(out, "%s", node->name);
            }
            break;
            
        case NODE_CONDITION_CMP:
            if (node->name && node->value) {
                fprintf(out, "%s %s", node->name, node->value);
            }
            break;
            
        case NODE_LIST:
            generate_ir_list(node->children, out);
            break;
            
        default:
            // For unknown node types, traverse children
            generate_ir_list(node->children, out);
            generate_ir_list(node->next, out);
            break;
    }
}

void optimize_ir(ASTNode* node) {
    if (!node) return;
    
    // Constant folding optimization
    if (node->type == NODE_EXPR_OP) {
        // Check if both operands are constants
        if (node->children && node->children->type == NODE_EXPR_VAL &&
            node->next && node->next->type == NODE_EXPR_VAL) {
            
            float left = atof(node->children->value);
            float right = atof(node->next->value);
            float result = 0;
            
            if (strcmp(node->name, "+") == 0) result = left + right;
            else if (strcmp(node->name, "-") == 0) result = left - right;
            else if (strcmp(node->name, "*") == 0) result = left * right;
            else if (strcmp(node->name, "/") == 0) result = left / right;
            else if (strcmp(node->name, ">") == 0) result = left > right;
            else if (strcmp(node->name, "<") == 0) result = left < right;
            else if (strcmp(node->name, "==") == 0) result = left == right;
            else if (strcmp(node->name, "!=") == 0) result = left != right;
            
            // Replace with constant
            char result_str[32];
            if (strcmp(node->name, ">") == 0 || strcmp(node->name, "<") == 0 ||
                strcmp(node->name, "==") == 0 || strcmp(node->name, "!=") == 0) {
                sprintf(result_str, "%d", (int)result);
            } else {
                sprintf(result_str, "%g", result);
            }
            
            node->type = NODE_EXPR_VAL;
            if (node->value) free(node->value);
            node->value = strdup(result_str);
            free_ast(node->children);
            free_ast(node->next);
            node->children = NULL;
            node->next = NULL;
        }
    }
    
    optimize_ir(node->children);
    optimize_ir(node->next);
}