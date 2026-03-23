%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"
#include "semantic.h"

extern int yylex();
extern int line_num;
extern int error_count;
extern Symbol symbol_table[MAX_SYMBOLS];
void yyerror(const char *s);

ASTNode* root = NULL;
%}

%define parse.error verbose
%define parse.lac full

%union {
    char* str;
    ASTNode* node;
}

%token ENTITY MODE STATE WHEN UNLESS IFNOT ELSE IF RETURN PRINT
%token LOOP_ORBIT LOOP_PULSE LOOP_BOUNCE LOOP_REPEAT LOOP_WHILE LOOP_FOR
%token TYPE_METER TYPE_MOOD TYPE_ENERGY TYPE_HEALTH TYPE_SPEED TYPE_POWER
%token TYPE_INT TYPE_FLOAT TYPE_BOOL
%token BOOL_TRUE BOOL_FALSE
%token ARROW FATARROW
%token EQEQ NEQ GTE LTE GT LT
%token PLUS MINUS STAR SLASH MOD
%token AND OR NOT
%token EQUALS
%token SEMICOLON COMMA COLON
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET

%token <str> ID_LITERAL
%token <str> STRING_LITERAL
%token <str> INT_LITERAL
%token <str> FLOAT_LITERAL

%type <node> program entity entity_list entity_content
%type <node> declaration_list_opt declaration_list mode_list mode
%type <node> statement_list_opt statement_list statement
%type <node> declaration transition loop_block assignment if_statement print_statement
%type <node> expression int_val condition
%type <node> statement_block

%type <str> type
%type <str> comp_op

%left OR
%left AND
%left EQEQ NEQ
%left GT LT GTE LTE
%left PLUS MINUS
%left STAR SLASH MOD
%right NOT

%start program

%%

program:
    declaration_list_opt entity_list {
        $$ = create_program($2);
        root = $$;
    }
;

declaration_list_opt:
      { $$ = NULL; }
    | declaration_list { $$ = $1; }
;

entity_list:
      entity { $$ = $1; }
    | entity_list entity { $$ = append_node($1, $2); }
;

entity:
    ENTITY ID_LITERAL LBRACE entity_content RBRACE {
        $$ = create_entity($2, $4);
        free($2);
    }
;

entity_content:
      { $$ = NULL; }
    | declaration_list { $$ = $1; }
    | mode_list { $$ = $1; }
    | declaration_list mode_list { $$ = append_node($1, $2); }
;

declaration_list:
      declaration { $$ = $1; }
    | declaration_list declaration { $$ = append_node($1, $2); }
;

declaration:
    type ID_LITERAL EQUALS int_val SEMICOLON {
        if(find_symbol($2) != -1) {
            printf("Semantic Error at line %d: Variable '%s' already declared\n", line_num, $2);
        } else {
            float val = atof($4->value);
            if(strcmp($1, "bool") == 0) {
                if(val != 0 && val != 1) {
                    printf("Semantic Error at line %d: Boolean variable '%s' must be 0 or 1\n", line_num, $2);
                }
                add_symbol($2, $1, val, 1);
            } else {
                add_symbol($2, $1, val, 0);
            }
        }
        $$ = create_decl($1, $2, $4->value);
        free($1); free($2);
        free_ast($4);
    }
    | type ID_LITERAL SEMICOLON {
        if(find_symbol($2) != -1) {
            printf("Semantic Error at line %d: Variable '%s' already declared\n", line_num, $2);
        } else {
            if(strcmp($1, "bool") == 0) {
                add_symbol($2, $1, 0, 1);
            } else {
                add_symbol($2, $1, 0, 0);
            }
        }
        $$ = create_decl($1, $2, "0");
        free($1); free($2);
    }
;

mode_list:
      mode { $$ = $1; }
    | mode_list mode { $$ = append_node($1, $2); }
;

mode:
    MODE ID_LITERAL LBRACE statement_list_opt RBRACE {
        $$ = create_mode($2, $4);
        free($2);
    }
;

statement_list_opt:
      { $$ = NULL; }
    | statement_list { $$ = $1; }
;

statement_list:
      statement { $$ = $1; }
    | statement_list statement { $$ = append_node($1, $2); }
;

statement:
      transition
    | loop_block
    | assignment
    | if_statement
    | print_statement
;

statement_block:
      LBRACE statement_list_opt RBRACE { $$ = $2; }
    | statement { $$ = $1; }
;

if_statement:
    IF LPAREN condition RPAREN statement_block ELSE statement_block {
        ASTNode* if_node = create_transition(NODE_IF, $3->name, "if");
        if_node->children = $5;
        if_node->next = create_transition(NODE_ELSE, NULL, "else");
        if_node->next->children = $7;
        $$ = if_node;
        free_ast($3);
    }
    | IF LPAREN condition RPAREN statement_block {
        $$ = create_transition(NODE_IF, $3->name, "if");
        $$->children = $5;
        free_ast($3);
    }
;

print_statement:
    PRINT LPAREN expression RPAREN SEMICOLON {
        char* expr_str = expr_to_string($3);
        $$ = create_transition(NODE_PRINT, expr_str, "print");
        free(expr_str);
        free_ast($3);
    }
;

comp_op:
      GT { $$ = ">"; }
    | LT { $$ = "<"; }
    | GTE { $$ = ">="; }
    | LTE { $$ = "<="; }
    | EQEQ { $$ = "=="; }
    | NEQ { $$ = "!="; }
;



condition:
      expression {
        char* expr_str = expr_to_string($1);
        $$ = create_condition(expr_str);
        free(expr_str);
        free_ast($1);
    }
;

transition:
      WHEN condition ARROW ID_LITERAL SEMICOLON {
          $$ = create_transition(NODE_WHEN, $2->name, $4);
          free_ast($2);
          free($4);
      }
    | UNLESS condition ARROW ID_LITERAL SEMICOLON {
          $$ = create_transition(NODE_UNLESS, $2->name, $4);
          free_ast($2);
          free($4);
      }
    | IFNOT condition ARROW ID_LITERAL SEMICOLON {
          $$ = create_transition(NODE_IFNOT, $2->name, $4);
          free_ast($2);
          free($4);
      }
;

loop_block:
      LOOP_ORBIT int_val LBRACE statement_list_opt RBRACE {
          int iterations = atoi($2->value);
          $$ = create_loop(iterations, $4);
          free_ast($2);
      }
    | LOOP_PULSE int_val LBRACE statement_list_opt RBRACE {
          int iterations = atoi($2->value);
          $$ = create_loop(iterations, $4);
          free_ast($2);
      }
    | LOOP_BOUNCE int_val LBRACE statement_list_opt RBRACE {
          int iterations = atoi($2->value);
          $$ = create_loop(iterations, $4);
          free_ast($2);
      }
;

assignment:
    ID_LITERAL EQUALS expression SEMICOLON {
        check_variable_declared($1);
        int idx = find_symbol($1);
        if(idx != -1) {
            VarType var_type = symbol_table[idx].vtype;
            
            // Determine expression type (simplified for now)
            VarType expr_type = VAR_TYPE_FLOAT; // Default
            
            // Check assignment compatibility with implicit conversion
            check_assignment_type($1, NULL, expr_type);
            
            if(var_type == VAR_TYPE_BOOL) {
                printf("Type checking: Assigning to boolean variable %s\n", $1);
            }
        }
        char* expr_str = expr_to_string($3);
        $$ = create_assignment($1, expr_str);
        free($1);
        free(expr_str);
        free_ast($3);
    }
;
expression:
      int_val { $$ = $1; }
    | ID_LITERAL { 
          check_variable_declared($1);
          $$ = create_expr_var($1); 
          free($1); 
      }
    | STRING_LITERAL { $$ = create_expr_val($1); free($1); }
    | BOOL_TRUE { $$ = create_expr_val("true"); }
    | BOOL_FALSE { $$ = create_expr_val("false"); }
    | expression PLUS expression { $$ = create_expr_op("+", $1, $3); }
    | expression MINUS expression { $$ = create_expr_op("-", $1, $3); }
    | expression STAR expression { $$ = create_expr_op("*", $1, $3); }
    | expression SLASH expression { $$ = create_expr_op("/", $1, $3); }
    | expression MOD expression { $$ = create_expr_op("%", $1, $3); }
    | expression GT expression { $$ = create_expr_op(">", $1, $3); }
    | expression LT expression { $$ = create_expr_op("<", $1, $3); }
    | expression GTE expression { $$ = create_expr_op(">=", $1, $3); }
    | expression LTE expression { $$ = create_expr_op("<=", $1, $3); }
    | expression EQEQ expression { $$ = create_expr_op("==", $1, $3); }
    | expression NEQ expression { $$ = create_expr_op("!=", $1, $3); }
    | NOT expression { $$ = create_expr_op("!", NULL, $2); }
    | LPAREN expression RPAREN { $$ = $2; }
;

int_val:
      INT_LITERAL { 
          $$ = create_expr_val($1); 
          free($1); 
      }
    | FLOAT_LITERAL { 
          $$ = create_expr_val($1); 
          free($1); 
      }
;

type:
      TYPE_METER { $$ = strdup("meter"); }
    | TYPE_MOOD { $$ = strdup("mood"); }
    | TYPE_ENERGY { $$ = strdup("energy"); }
    | TYPE_HEALTH { $$ = strdup("health"); }
    | TYPE_SPEED { $$ = strdup("speed"); }
    | TYPE_POWER { $$ = strdup("power"); }
    | TYPE_INT { $$ = strdup("int"); }
    | TYPE_FLOAT { $$ = strdup("float"); }
    | TYPE_BOOL { $$ = strdup("bool"); }
;

%%

void yyerror(const char *s) {
    printf("\nSyntax Error at line %d: %s\n", line_num, s);
    error_count++;
}