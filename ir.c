#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "ast.h"

static int temp_counter = 0;
static int label_counter = 0;
static int loop_counter = 0;

// Helper function to check if a string is a number
static int is_number(const char* str) {
    if (!str || *str == '\0') return 0;
    
    int has_dot = 0;
    int i = 0;
    
    // Handle negative numbers
    if (str[0] == '-') i = 1;
    
    for (; str[i] != '\0'; i++) {
        if (isdigit(str[i])) continue;
        if (str[i] == '.' && !has_dot) {
            has_dot = 1;
            continue;
        }
        return 0;
    }
    return i > (str[0] == '-' ? 1 : 0);
}


static char* fold_constants_in_expression(const char* expr) {
    if (!expr || strlen(expr) == 0) return strdup(expr);
    
    // First, clean up unwanted characters (parentheses and semicolons)
    char temp[256];
    int j = 0;
    for (int i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];
        // Skip parentheses and semicolons
        if (c == '(' || c == ')' || c == ';') {
            continue;
        }
        temp[j++] = c;
    }
    temp[j] = '\0';
    
    char* result = malloc(256);
    strcpy(result, temp);
    
    // Keep folding until no more improvements
    int changed = 1;
    while (changed) {
        changed = 0;
        char* ptr = result;
        
        // Look for patterns: number operator number
        while (*ptr) {
            // Skip whitespace
            while (*ptr && isspace(*ptr)) ptr++;
            
            // Try to match: number op number
            char* start = ptr;
            char left_str[32] = {0};
            char op[3] = {0};
            char right_str[32] = {0};
            
            // Parse left number
            int k = 0;
            if (*ptr == '-' || *ptr == '+') {
                left_str[k++] = *ptr++;
            }
            while (*ptr && isdigit(*ptr)) {
                left_str[k++] = *ptr++;
            }
            if (*ptr == '.') {
                left_str[k++] = *ptr++;
                while (*ptr && isdigit(*ptr)) {
                    left_str[k++] = *ptr++;
                }
            }
            left_str[k] = '\0';
            
            if (is_number(left_str)) {
                // Skip whitespace
                while (*ptr && isspace(*ptr)) ptr++;
                
                // Parse operator
                k = 0;
                if (*ptr && strchr("+-*/%", *ptr)) {
                    op[k++] = *ptr++;
                    op[k] = '\0';
                } else {
                    ptr = start + 1;
                    continue;
                }
                
                // Skip whitespace
                while (*ptr && isspace(*ptr)) ptr++;
                
                // Parse right number
                k = 0;
                if (*ptr == '-' || *ptr == '+') {
                    right_str[k++] = *ptr++;
                }
                while (*ptr && isdigit(*ptr)) {
                    right_str[k++] = *ptr++;
                }
                if (*ptr == '.') {
                    right_str[k++] = *ptr++;
                    while (*ptr && isdigit(*ptr)) {
                        right_str[k++] = *ptr++;
                    }
                }
                right_str[k] = '\0';
                
                if (is_number(right_str)) {
                    // We have: number op number - fold them!
                    float left = atof(left_str);
                    float right = atof(right_str);
                    float res = 0;
                    int valid = 1;
                    
                    if (op[0] == '+') res = left + right;
                    else if (op[0] == '-') res = left - right;
                    else if (op[0] == '*') res = left * right;
                    else if (op[0] == '/') {
                        if (right != 0) res = left / right;
                        else valid = 0;
                    }
                    else if (op[0] == '%') {
                        res = (int)left % (int)right;
                    }
                    
                    if (valid) {
                        // Create the folded result
                        char folded[32];
                        if (res == (int)res) {
                            sprintf(folded, "%.0f", res);
                        } else {
                            sprintf(folded, "%g", res);
                        }
                        
                        // Rebuild the string with the folded result
                        char new_result[256] = {0};
                        strncat(new_result, result, start - result);
                        strcat(new_result, folded);
                        strcat(new_result, ptr);
                        strcpy(result, new_result);
                        
                        changed = 1;
                        ptr = result;
                    } else {
                        ptr = start + 1;
                    }
                } else {
                    ptr = start + 1;
                }
            } else {
                ptr = start + 1;
            }
        }
    }
    
    return result;
}

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

    // Recursively optimize children and next nodes first
    optimize_ir(node->children);
    optimize_ir(node->next);

    // Optimize expressions in assignments
    if (node->type == NODE_ASSIGN && node->value) {
        char* optimized = fold_constants_in_expression(node->value);
        if (strcmp(optimized, node->value) != 0) {
            free(node->value);
            node->value = optimized;
        } else {
            free(optimized);
        }
    }
    
    // Optimize expressions in declarations
    if (node->type == NODE_DECL && node->value) {
        char* optimized = fold_constants_in_expression(node->value);
        if (strcmp(optimized, node->value) != 0) {
            free(node->value);
            node->value = optimized;
        } else {
            free(optimized);
        }
    }

    // Fold operator nodes (AST-level optimization)
    if (node->type == NODE_EXPR_OP && node->children && node->children->type == NODE_EXPR_VAL) {
        ASTNode* leftNode = node->children;
        ASTNode* rightNode = leftNode->next; // Right operand is usually children->next

        if (rightNode && rightNode->type == NODE_EXPR_VAL) {
            float left = atof(leftNode->value);
            float right = atof(rightNode->value);
            float result = 0;

            // Handle arithmetic
            if (strcmp(node->name, "+") == 0) result = left + right;
            else if (strcmp(node->name, "-") == 0) result = left - right;
            else if (strcmp(node->name, "*") == 0) result = left * right;
            else if (strcmp(node->name, "/") == 0 && right != 0) result = left / right;
            else if (strcmp(node->name, "%") == 0 && right != 0) result = (int)left % (int)right;

            // Convert result to string
            char result_str[32];
            if (result == (int)result) {
                sprintf(result_str, "%.0f", result);
            } else {
                sprintf(result_str, "%g", result);
            }

            // Replace operator node with constant value
            node->type = NODE_EXPR_VAL;
            free(node->value);
            node->value = strdup(result_str);

            // Free old children nodes
            free_ast(leftNode);
            free_ast(rightNode);

            node->children = NULL;
        }
    }
}