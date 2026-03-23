#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static int temp_counter = 0;
static int label_counter = 0;

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

void generate_ir(ASTNode* node, FILE* out) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            fprintf(out, "; Three-Address Code (Intermediate Representation)\n");
            fprintf(out, "; ============================================\n\n");
            generate_ir(node->children, out);
            break;
            
        case NODE_ENTITY:
            fprintf(out, "; Entity: %s\n", node->name ? node->name : "unknown");
            generate_ir(node->children, out);
            break;
            
        case NODE_MODE:
            fprintf(out, "\n; Mode: %s\n", node->name ? node->name : "unknown");
            fprintf(out, "label_%s:\n", node->name);
            generate_ir(node->children, out);
            break;
            
        case NODE_DECL:
            fprintf(out, "DECL %s = %s\n", node->name, node->value);
            break;
            
        case NODE_ASSIGN:
            fprintf(out, "%s = %s\n", node->name, node->value);
            break;
            
        case NODE_PRINT:
            fprintf(out, "print %s\n", node->name);
            break;
            
        case NODE_WHEN:
            fprintf(out, "if %s goto %s\n", node->name, node->value);
            break;
            
        case NODE_IF:
            fprintf(out, "if %s goto L%d\n", node->name, label_counter);
            generate_ir(node->children, out);
            if (node->next) {
                fprintf(out, "goto L%d\n", label_counter + 1);
                fprintf(out, "L%d:\n", label_counter);
                generate_ir(node->next->children, out);
                fprintf(out, "L%d:\n", label_counter + 1);
                label_counter += 2;
            } else {
                fprintf(out, "L%d:\n", label_counter++);
            }
            break;
            
        case NODE_LOOP:
            {
                char* start_label = new_label();
                char* end_label = new_label();
                fprintf(out, "%s:\n", start_label);
                fprintf(out, "if %d <= 0 goto %s\n", node->loop_count, end_label);
                generate_ir(node->children, out);
                fprintf(out, "%s = %s - 1\n", "counter", "counter");
                fprintf(out, "goto %s\n", start_label);
                fprintf(out, "%s:\n", end_label);
                free(start_label);
                free(end_label);
            }
            break;
            
        case NODE_EXPR_VAL:
            fprintf(out, "%s", node->value);
            break;
            
        case NODE_EXPR_VAR:
            fprintf(out, "%s", node->name);
            break;
            
        case NODE_EXPR_OP:
            {
                char* temp = new_temp();
                fprintf(out, "%s = ", temp);
                if (node->children) generate_ir(node->children, out);
                fprintf(out, " %s ", node->name);
                if (node->next) generate_ir(node->next, out);
                fprintf(out, "\n");
                free(temp);
            }
            break;
            
        default:
            generate_ir(node->children, out);
            generate_ir(node->next, out);
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
            
            // Replace with constant
            char result_str[32];
            sprintf(result_str, "%g", result);
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