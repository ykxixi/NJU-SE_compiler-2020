%{
    #include "syntax.tab.h"
    #include "AstNode.h"
%}
%option yylineno
digit	[0-9]
letter	[_a-zA-Z]
hexadecimal  0[xX][0-9a-fA-F]+
octal        0[1-7][0-7]+
decimal      [1-9]+[0-9]*|0
number       ({digit}*\.{digit}+|{digit}+\.)([eE]?[+-]?{digit}+)
id           {letter}({letter}|{digit})*
t	"*/"

%%
int|float	{
			yylval.astnode = setAstNode("TYPE", 0, yylineno, yytext);
			return TYPE;
		}
if		{
			yylval.astnode = setAstNode("IF", 0, yylineno);
			return IF;
		}
else		{
			yylval.astnode = setAstNode("ELSE", 0, yylineno);
			return ELSE;
		}
while		{
			yylval.astnode = setAstNode("WHILE", 0, yylineno);
			return WHILE;
		}
struct		{
			yylval.astnode = setAstNode("STRUCT", 0, yylineno);
			return STRUCT;
		}
return		{
			yylval.astnode = setAstNode("RETURN", 0, yylineno);
			return RETURN;
		}
[ \t\v\f\r\n]	{}
";"		{
			yylval.astnode = setAstNode("SEMI", 0, yylineno);
			return SEMI;
		}
"("		{
			yylval.astnode = setAstNode("LP", 0, yylineno);
			return LP;
		}
")"		{
			yylval.astnode = setAstNode("RP", 0, yylineno);
			return RP;
		}
"["		{
			yylval.astnode = setAstNode("LB", 0, yylineno);
			return LB;
		}
"]"		{
			yylval.astnode = setAstNode("RB", 0, yylineno);
			return RB;
		}
"{"		{
			yylval.astnode = setAstNode("LC", 0, yylineno);
			return LC;
		}
"}"		{
			yylval.astnode = setAstNode("RC", 0, yylineno);
			return RC;
		}
"+"		{
			yylval.astnode = setAstNode("PLUS", 0, yylineno);
			return PLUS;
		}
"-"		{
			yylval.astnode = setAstNode("MINUS", 0, yylineno);
			return MINUS;
		}
"*"		{
			yylval.astnode = setAstNode("STAR", 0, yylineno);
			return STAR;
		}
"//"		{char c = input(); while(c != '\n') c = input();}
(\/\*)[^{t}]*(\*\/)	{}
"/"		{
			yylval.astnode = setAstNode("DIV", 0, yylineno);
			return DIV;
		}
"."		{
			yylval.astnode = setAstNode("DOT", 0, yylineno);
			return DOT;
		}
","		{
			yylval.astnode = setAstNode("COMMA", 0, yylineno);
			return COMMA;
		}
"!="		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
"!"		{
			yylval.astnode = setAstNode("NOT", 0, yylineno);
			return NOT;
		}
"||"		{
			yylval.astnode = setAstNode("OR", 0, yylineno);
			return OR;
		}
"&&"		{
			yylval.astnode = setAstNode("AND", 0, yylineno);
			return AND;
		}
">"		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
"<"		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
"=="		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
">="		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
"<="		{
			yylval.astnode = setAstNode("RELOP", 0, yylineno);
			return RELOP;
		}
"="		{
			yylval.astnode = setAstNode("ASSIGNOP", 0, yylineno);
			return ASSIGNOP;
		}

{number} 	{
			yylval.astnode = setAstNode("FLOAT", 0, yylineno, atof(yytext));
			return FLOAT;
		}
	
{decimal} 	{
			yylval.astnode = setAstNode("INT", 0, yylineno, atol(yytext));
			return INT;
		}
		
{octal} 	{
			int i = strlen(yytext) - 1;
			unsigned int temp = 0;
			int idx=1;
			for(i; i >= 1; i--){
				temp += (yytext[i] - 48) * idx;
				idx *= 8;
		    	}
			yylval.astnode = setAstNode("INT", 0, yylineno, temp);
			return INT;
		}
				
{hexadecimal}	{
			int i = strlen(yytext) - 1;
			unsigned int temp = 0;
			int idx = 1;
			for(i; i >= 2; i--){
				if(yytext[i] >= 65)
					if(yytext[i] >= 97) temp += (yytext[i] - 55 - 32) * idx;
				    	else temp += (yytext[i] - 55) * idx;
				else
				    temp += (yytext[i] - 48) * idx;
				idx *= 16;
		    	}
		    	yylval.astnode = setAstNode("INT", 0, yylineno, temp);
			return INT;
}
		
{id}		{
			yylval.astnode = setAstNode("ID", 0, yylineno, yytext);
			return ID;
		}

.		{ fprintf(stderr,"Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext); }

%%
