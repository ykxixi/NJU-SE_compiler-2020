%{
    #include "syntax.tab.h"
    #include<stdio.h>
    #include<string.h>
    int err;
    extern FILE* oFile;
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
{ws}         	{}
{type}       	{ fprintf(oFile, "TYPE %s at Line %d.\n", yytext, yylineno); }
{keyword}    	{
			int i = 0;
	    		/* 转换为大写 */
    			while(yytext[i] != '\0'){
    				if(yytext[i] <= 'z' && yytext[i] >= 'a'){
    					fprintf(oFile, "%c", yytext[i++] - ('a' - 'A'));
    				} else {
    					fprintf(oFile, "%c", yytext[i++]);
    				}
			}
        		fprintf(oFile, " %s at Line %d.\n", yytext, yylineno);
		}
"//"            {
			char c = input();
			while (c != '\n') {
				c = input();
			}
		}
"/*"[^("*/")]*"*/" {}
{relop}		{ fprintf(oFile, "RELOP %s at Line %d.\n", yytext, yylineno); }
";"         	{ fprintf(oFile, "SEMI %s at Line %d.\n", yytext, yylineno); }
","     	{ fprintf(oFile, "COMMA %s at Line %d.\n", yytext, yylineno); }
"="        	{ fprintf(oFile, "ASSIGNOP %s at Line %d.\n", yytext, yylineno); }	
"+"		{ fprintf(oFile, "PLUS %s at Line %d.\n", yytext, yylineno); }
"-"		{ fprintf(oFile, "MINUS %s at Line %d.\n", yytext, yylineno); }
"*"		{ fprintf(oFile, "STAR %s at Line %d.\n", yytext, yylineno); }
"/"		{ fprintf(oFile, "DIV %s at Line %d.\n", yytext, yylineno); }
"&&"		{ fprintf(oFile, "AND %s at Line %d.\n", yytext, yylineno); }
"||"		{ fprintf(oFile, "OR %s at Line %d.\n", yytext, yylineno); }
"."		{ fprintf(oFile, "DOT %s at Line %d.\n", yytext, yylineno); }
"!"		{ fprintf(oFile, "NOT %s at Line %d.\n", yytext, yylineno); }
"("		{ fprintf(oFile, "LP %s at Line %d.\n", yytext, yylineno); }
")"		{ fprintf(oFile, "RP %s at Line %d.\n", yytext, yylineno); }
"["		{ fprintf(oFile, "LB %s at Line %d.\n", yytext, yylineno); }
"]"		{ fprintf(oFile, "RB %s at Line %d.\n", yytext, yylineno); }
"{"		{ fprintf(oFile, "LC %s at Line %d.\n", yytext, yylineno); }
"}"		{ fprintf(oFile, "RC %s at Line %d.\n", yytext, yylineno); }
{hexadecimal}   { 
			char* endPtr = NULL;
			long hex = strtol(yytext, &endPtr, 16);
/*
			int i = strlen(yytext) - 1;
			unsigned int hex = 0;
			int idx = 1;
			for(; i >= 2; i--){
				if(yytext[i] >= 'a') {
					hex += (yytext[i] - 'a') * idx;
				} else if(yytext[i] >= 'A') {
				    	hex += (yytext[i] - 'A') * idx;
				} else {
				    	hex += (yytext[i] - '0') * idx;
				}
				idx *= 16;
		    	}
*/
			fprintf(oFile, "INT %ld at Line %d.\n", hex, yylineno); }
{octal}		{ 
			char* endPtr = NULL;
			long oc = strtol(yytext, &endPtr, 8);
/*
			int i = strlen(yytext) - 1;
			unsigned int oc = 0;
			int idx = 1;
			for(; i >= 1; i--){
				oc += (yytext[i]-'0') * idx;
				idx *= 8;
		    	}
*/
			fprintf(oFile, "INT %ld at Line %d.\n", oc, yylineno); 
		}
{decimal}   	{ fprintf(oFile, "INT %ld at Line %d.\n", atol(yytext), yylineno); }              //integer要放在id前，否则十六进制会被识别为id
{number}	{ fprintf(oFile, "FLOAT %f at Line %d.\n", atof(yytext), yylineno); }
{id}		{ fprintf(oFile, "ID %s at Line %d.\n", yytext, yylineno); }
.		{ 
			err = 1;
			fprintf(stderr, "Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext); 
		}
