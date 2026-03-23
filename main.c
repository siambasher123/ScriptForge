#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symbol_table.h"
#include "semantic.h"

extern FILE* yyin;
extern int yyparse();
extern ASTNode* root;
extern int error_count;
extern void init_builtin_variables();
extern void generate_ir(ASTNode* node, FILE* out);
extern void optimize_ir(ASTNode* node);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    
    FILE* input = fopen(argv[1], "r");
    if (!input) {
        printf("Cannot open input file: %s\n", argv[1]);
        return 1;
    }
    
    yyin = input;
    init_builtin_variables();
    error_count = 0;
    
    printf("========================================\n");
    printf("GOBLIN AI COMPILER\n");
    printf("========================================\n\n");
    
    printf("Parsing %s...\n", argv[1]);
    int result = yyparse();
    fclose(input);
    
    if (result == 0 && error_count == 0) {
        printf("✓ Parsing successful!\n\n");
        
        printf("Abstract Syntax Tree:\n");
        printf("--------------------\n");
        print_ast(root, 0);
        printf("\n");
        
        print_symbol_table();
        
        // Apply optimizations
        printf("\nApplying optimizations...\n");
        optimize_ir(root);
        printf("✓ Optimizations applied\n\n");
        
        // Generate intermediate code
        printf("Generating Intermediate Code (Three-Address Code)...\n");
        FILE* ir_file = fopen("output.ir", "w");
        if (ir_file) {
            generate_ir(root, ir_file);
            fclose(ir_file);
            printf("✓ Intermediate code generated to output.ir\n");
        }
        
        // Generate C# code
        printf("\nGenerating Unity C# code...\n");
        FILE* output = fopen("output.cs", "w");
        if (output) {
            generate_csharp(root, output);
            fclose(output);
            printf("✓ C# code generated to output.cs\n");
        } else {
            printf("✗ Error: Cannot create output file\n");
        }
        
        printf("\n========================================\n");
        printf("COMPILATION COMPLETE\n");
        printf("========================================\n");
        printf("Generated files:\n");
        printf("  - output.cs  (Unity C# code)\n");
        printf("  - output.ir  (Intermediate representation)\n");
        printf("========================================\n");
        
        free_ast(root);
    } else {
        printf("\n✗ Compilation failed with %d error(s)!\n", error_count);
    }
    
    return 0;
}