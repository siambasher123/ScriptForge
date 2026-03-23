%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int line_num = 1;
int error_count = 0;
%}

DIGIT      [0-9]
LETTER     [a-zA-Z_]
ID         {LETTER}({LETTER}|{DIGIT})*
INT        {DIGIT}+
FLOAT      {DIGIT}+"."{DIGIT}+

%x COMMENT

%%

\n                      { line_num++; }
[ \t\r]+                { }

"//".*                  { }

"/*"                    { BEGIN(COMMENT); }

<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n             { line_num++; }
<COMMENT>.              { }

"entity"                { return ENTITY; }
"mode"                  { return MODE; }
"state"                 { return STATE; }
"when"                  { return WHEN; }
"unless"                { return UNLESS; }
"ifnot"                 { return IFNOT; }
"else"                  { return ELSE; }
"if"                    { return IF; }
"print"                 { return PRINT; }
"return"                { return RETURN; }

"orbit"                 { return LOOP_ORBIT; }
"pulse"                 { return LOOP_PULSE; }
"bounce"                { return LOOP_BOUNCE; }
"repeat"                { return LOOP_REPEAT; }
"while"                 { return LOOP_WHILE; }
"for"                   { return LOOP_FOR; }

"meter"                 { return TYPE_METER; }
"mood"                  { return TYPE_MOOD; }
"energy"                { return TYPE_ENERGY; }
"health"                { return TYPE_HEALTH; }
"speed"                 { return TYPE_SPEED; }
"power"                 { return TYPE_POWER; }
"int"                   { return TYPE_INT; }
"float"                 { return TYPE_FLOAT; }
"bool"                  { return TYPE_BOOL; }

"true"                  { yylval.str = strdup("true"); return BOOL_TRUE; }
"false"                 { yylval.str = strdup("false"); return BOOL_FALSE; }

"->"                    { return ARROW; }
"=>"                    { return FATARROW; }
"=="                    { return EQEQ; }
"!="                    { return NEQ; }
">="                    { return GTE; }
"<="                    { return LTE; }
">"                     { return GT; }
"<"                     { return LT; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return STAR; }
"/"                     { return SLASH; }
"%"                     { return MOD; }
"&&"                    { return AND; }
"||"                    { return OR; }
"!"                     { return NOT; }
"="                     { return EQUALS; }

";"                     { return SEMICOLON; }
","                     { return COMMA; }
":"                     { return COLON; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }

{FLOAT}                 { yylval.str = strdup(yytext); return FLOAT_LITERAL; }
{INT}                   { yylval.str = strdup(yytext); return INT_LITERAL; }

\"([^\\\"]|\\.)*\"      { 
    yylval.str = strdup(yytext); 
    return STRING_LITERAL; 
}

{ID}                    { yylval.str = strdup(yytext); return ID_LITERAL; }

.                       { 
    printf("Lexical Error at line %d: Invalid character '%s'\n", line_num, yytext);
    error_count++;
}

%%

int yywrap()
{
    return 1;
}