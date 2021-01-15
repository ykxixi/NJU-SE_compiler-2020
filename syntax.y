%{
/*
    void yyerror(const char* msg);
    int yyparse(void);
*/
    #include "lex.yy.c"
    #include "AstNode.h"
%}

/* 类型 */
%union {
  struct AstNode* astnode;
}

/* 词法单元 */
%token <astnode> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

/* 语法单元 */
%type <astnode> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

/* 优先级 */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS
%left PLUS
%left DIV
%left STAR
%right NOT 
%right UMINUS
%left DOT
%left LC RC
%left LB RB
%left LP RP

%%
/* High-level Definitions */
Program : ExtDefList 				{ $$ = setAstNode("Program", 1, $1); traversal($$, 0); }
    	;
ExtDefList: ExtDef ExtDefList 			{ $$ = setAstNode("ExtDefList", 2, $1, $2); }
	| /* epsilon */				{ $$ = setAstNode("ExtDefList", 0, -1); }
	;
ExtDef : Specifier ExtDecList SEMI    		{ $$ = setAstNode("ExtDef", 3, $1, $2, $3); }    
	| Specifier SEMI			{ $$ = setAstNode("ExtDef", 2, $1, $2); }
	| Specifier FunDec CompSt		{ $$ = setAstNode("ExtDef", 3, $1, $2, $3); }
	;
ExtDecList : VarDec 				{ $$ = setAstNode("ExtDecList", 1, $1); }
	| VarDec COMMA ExtDecList 		{ $$ = setAstNode("ExtDecList", 3, $1, $2, $3); }
	;

/* Specifiers */
Specifier : TYPE 				{ $$ = setAstNode("Specifier", 1, $1); }
	| StructSpecifier 			{ $$ = setAstNode("Specifier", 1, $1); }
	;
StructSpecifier : STRUCT OptTag LC DefList RC 	{ $$ = setAstNode("StructSpecifier", 5, $1, $2, $3, $4, $5); }
	| STRUCT Tag 				{ $$ = setAstNode("StructSpecifier", 2, $1, $2); }
	;
OptTag : ID 					{ $$ = setAstNode("OptTag", 1, $1); }
	| /* epsilon */				{ $$ = setAstNode("OptTag", 0, -1); }
	;
Tag : ID 					{ $$ = setAstNode("Tag", 1, $1); }
	;

/* Declarators */
VarDec : ID 					{ $$ = setAstNode("VarDec", 1, $1); }
	| VarDec LB INT RB 			{ $$ = setAstNode("VarDec", 4, $1, $2, $3, $4); }
	;
FunDec : ID LP VarList RP			{ $$ = setAstNode("FunDec", 4, $1, $2, $3, $4); }
	| ID LP RP 				{ $$ = setAstNode("FunDec", 3, $1, $2, $3); }
	;
VarList : ParamDec COMMA VarList 		{ $$ = setAstNode("VarList", 3, $1, $2, $3); }
	| ParamDec 				{ $$ = setAstNode("VarList", 1, $1); }
	;
ParamDec : Specifier VarDec 			{ $$ = setAstNode("ParamDec", 2, $1, $2); }
        ;

/* Statement */
CompSt : LC DefList StmtList RC 		{ $$ = setAstNode("CompSt", 4, $1, $2, $3, $4); }
	;
StmtList:Stmt StmtList				{ $$ = setAstNode("StmtList", 2, $1, $2); }
	| /* epsilon */ 			{ $$ = setAstNode("StmtList", 0, -1); }
	;
Stmt : Exp SEMI				 	{ $$ = setAstNode("Stmt", 2, $1, $2); }
	| CompSt 				{ $$ = setAstNode("Stmt", 1, $1); }
	| RETURN Exp SEMI 			{ $$ = setAstNode("Stmt", 3, $1, $2, $3); }
	| IF LP Exp RP Stmt 			{ $$ = setAstNode("Stmt", 5, $1, $2, $3, $4, $5); }
	| IF LP Exp RP Stmt ELSE Stmt 		{ $$ = setAstNode("Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
	| WHILE LP Exp RP Stmt 			{ $$ = setAstNode("Stmt", 5, $1, $2, $3, $4, $5); }
	;

/* Local Definitions */
DefList : Def DefList				{ $$ = setAstNode("DefList", 2, $1, $2); }
	| /* epsilon */				{ $$ = setAstNode("DefList", 0, -1); }
	;
Def : Specifier DecList SEMI 			{ $$ = setAstNode("Def", 3, $1, $2, $3); }
	;
DecList : Dec					{ $$ = setAstNode("DecList", 1, $1); }
	| Dec COMMA DecList 			{ $$ = setAstNode("DecList", 3, $1, $2, $3); }
	;
Dec : VarDec 					{ $$ = setAstNode("Dec", 1, $1); }
	| VarDec ASSIGNOP Exp 			{ $$ = setAstNode("Dec", 3, $1, $2, $3); }
	;

/* Expressions */
Exp : Exp ASSIGNOP Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp AND Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp OR Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp RELOP Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp PLUS Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp MINUS Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp STAR Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp DIV Exp				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | LP Exp RP				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | MINUS Exp				{ $$ = setAstNode("Exp", 2, $1, $2); }
        | NOT Exp 				{ $$ = setAstNode("Exp", 2, $1, $2); }
        | ID LP Args RP 			{ $$ = setAstNode("Exp", 4, $1, $2, $3, $4); }
        | ID LP RP 				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp LB Exp RB 			{ $$ = setAstNode("Exp", 4, $1, $2, $3, $4); }
        | Exp DOT ID 				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | ID					{ $$ = setAstNode("Exp", 1, $1); }
        | INT					{ $$ = setAstNode("Exp", 1, $1); }
        | FLOAT					{ $$ = setAstNode("Exp", 1, $1); }
        ;
Args : Exp COMMA Args 				{ $$ = setAstNode("Exp", 3, $1, $2, $3); }
        | Exp 					{ $$ = setAstNode("Exp", 1, $1); }
        ;

