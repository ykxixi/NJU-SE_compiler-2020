%{
	#include "AstNode.h"
    #include "syntax.tab.h"
    int err;
%}
%option      yylineno
relop        ">"|"<"|">="|"<="|"=="|"!="
type         "int"|"float"
keyword      "struct"|"return"|"if"|"else"|"while"
ws           [ \t\v\f\n\r]+
digit        [0-9]
letter       [a-zA-Z_]
hexadecimal  0[xX][0-9a-fA-F]+
octal        0[1-7][0-7]+
decimal      [1-9]+[0-9]*|0
number       ({digit}*\.{digit}+|{digit}+\.)([eE]?[+-]?{digit}+)
id           {letter}({letter}|{digit})*
%%
{ws}         		{}
{type}       		{
						yylval.astnode = setAstNode("TYPE", 0, yylineno, yytext);
						return TYPE;
					}
"if"				{
						yylval.astnode = setAstNode("IF", 0, yylineno);
						return IF;
					}
"else"				{
						yylval.astnode = setAstNode("ELSE", 0, yylineno);
						return ELSE;
					}
"while"				{
						yylval.astnode = setAstNode("WHILE", 0, yylineno);
						return WHILE;
					}
"struct"			{
						yylval.astnode = setAstNode("STRUCT", 0, yylineno);
						return STRUCT;
					}
"return"			{
						yylval.astnode = setAstNode("RETURN", 0, yylineno);
						return RETURN;
					}
"//"            	{
						char c = input();
						while (c != '\n') {
							c = input();
						}
					}
"/*"[^("*/")]*"*/" 	{}
{relop}				{ 
						yylval.astnode = setAstNode("RELOP", 0, yylineno);
						return RELOP;
 					}
";"					{
						yylval.astnode = setAstNode("SEMI", 0, yylineno);
						return SEMI;
					}
","					{
						yylval.astnode = setAstNode("COMMA", 0, yylineno);
						return COMMA;
					}
"="					{
						yylval.astnode = setAstNode("ASSIGNOP", 0, yylineno);
						return ASSIGNOP;
					}	
"+"					{
						yylval.astnode = setAstNode("PLUS", 0, yylineno);
						return PLUS;
					}
"-"					{
						yylval.astnode = setAstNode("MINUS", 0, yylineno);
						return MINUS;
					}
"*"					{
						yylval.astnode = setAstNode("STAR", 0, yylineno);
						return STAR;
					}
"/"					{
						yylval.astnode = setAstNode("DIV", 0, yylineno);
						return DIV;
					}
"||"				{
						yylval.astnode = setAstNode("OR", 0, yylineno);
						return OR;
					}
"&&"				{
						yylval.astnode = setAstNode("AND", 0, yylineno);
						return AND;
					}
"."					{
						yylval.astnode = setAstNode("DOT", 0, yylineno);
						return DOT;
					}
"!"					{
						yylval.astnode = setAstNode("NOT", 0, yylineno);
						return NOT;
					}
"("					{
						yylval.astnode = setAstNode("LP", 0, yylineno);
						return LP;
					}
")"					{
						yylval.astnode = setAstNode("RP", 0, yylineno);
						return RP;
					}
"["					{
						yylval.astnode = setAstNode("LB", 0, yylineno);
						return LB;
					}
"]"					{
						yylval.astnode = setAstNode("RB", 0, yylineno);
						return RB;
					}
"{"					{
						yylval.astnode = setAstNode("LC", 0, yylineno);
						return LC;
					}
"}"					{
						yylval.astnode = setAstNode("RC", 0, yylineno);
						return RC;
					}
{hexadecimal}   	{ 
						char* endPtr = NULL;
						long hex = strtol(yytext, &endPtr, 16);
						yylval.astnode = setAstNode("INT", 0, yylineno, hex);
						return INT;
					}
{octal}				{ 
						char* endPtr = NULL;
						long oct = strtol(yytext, &endPtr, 8);
						yylval.astnode = setAstNode("INT", 0, yylineno, oct);
						return INT;
					}
{decimal}   		{
						yylval.astnode = setAstNode("INT", 0, yylineno, atol(yytext));
						return INT;
					}
{number} 			{
						yylval.astnode = setAstNode("FLOAT", 0, yylineno, atof(yytext));
						/* yylval.astnode = setAstNode("FLOAT", 0, yylineno, yytext); */
						return FLOAT;
					}
{id}				{
						yylval.astnode = setAstNode("ID", 0, yylineno, yytext);
						return ID;
					}
.					{ 
						err = 1;
						fprintf(stderr, "Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext); 
					}
