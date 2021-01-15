#ifndef SEMANTIC_H
#define SEMANTIC_H

#include"AstNode.h"
#include"type.h"

// 错误类型
#define VAR_NOT_DEFINED 1
#define FUNC_NOT_DEFINED 2
#define VAR_REDEFINED 3
#define FUNC_REDEFINED 4
#define ASSIGNOP_UNMATCHED 5
#define RIGHT_VALUE_LEFT 6
#define OPER_UNMATCHED 7
#define RETURN_UNMATCHED 8
#define FUNC_PARAM_UNMATCHED 9
#define WRONG_ARRAY 10
#define WRONG_FUNC 11
#define ARRAY_IDX_NOT_INT 12
#define WRONG_STRUCT 13
#define STRUCT_FIELD_NOT_DEFINED 14
#define ERROR_STRUCT_FIELD 15
#define INIT_STRUCT 18
#define STURCT_REDEFINED 16
#define STRUCT_NOT_DEFINED 17

// High-level Definitions
extern char* EXT_DEF_LIST;
extern char* EXT_DEF;
extern char* EXT_DEC_LIST;

// Specifiers
extern char* SPECIFIER;
extern char* STRUCT_SPECIFIER;
extern char* OPT_TAG;
extern char* TAG;

// Declarators
extern char* VAR_DEC;
extern char* FUN_DEC;
extern char* VAR_LIST;
extern char* PARAM_DEC;

// Statements
extern char* COMP_ST;
extern char* STMT_LIST;
extern char* STMT;

// Local Definitions
extern char* DEF_LIST;
extern char* DEF;
extern char* DEC_LIST;
extern char* DEC;

// Expressions
extern char* EXP;
extern char* ARGS;

// Tokens
extern char* TER_SEMI;
extern char* TER_ID;
extern char* TER_TYPE;
extern char* TER_ASSIGNOP;
extern char* TER_AND;
extern char* TER_OR;
extern char* TER_RELOP;
extern char* TER_PLUS;
extern char* TER_MINUS;
extern char* TER_STAR;
extern char* TER_DIV;
extern char* TER_LP;
extern char* TER_RP;
extern char* TER_LB;
extern char* TER_RB;
extern char* TER_NOT;
extern char* TER_DOT;
extern char* TER_INT;
extern char* TER_FLOAT;
extern char* TER_RETURN;
extern char* TER_IF;
extern char* TER_ELSE;
extern char* TER_WHILE;


// High-level Definitions
extern void ExtDefList(AstNode* extDefListNode);
extern void ExtDef(AstNode* extDefNode);
extern void ExtDecList(AstNode* extDecListNode, Type type);

// Specifiers
Type Specifier(AstNode* specifierNode);
Type StructSpecifier(AstNode* structSpecifierNode);

// Declarators
FieldList VarDec(AstNode* varDecNode, Type type, int isForStruct);
Function FunDec(AstNode* funDecNode, Type type);
FieldList VarList(AstNode* varListNode);
FieldList ParamDec(AstNode* paramDecNode);

// Statements
void CompSt(AstNode* compStNode, Type type);
void StmtList(AstNode* stmtListNode, Type type);
void Stmt(AstNode* stmtNode, Type type);

// Local Definitions
FieldList DefList(AstNode* defListNode, int isForStruct);
FieldList Def(AstNode* defNode, int isForStruct);
FieldList DecList(AstNode* decListNode, Type type, int isForStruct);
FieldList Dec(AstNode* decNode, Type type, int isForStruct);

// Expressions
Type Exp(AstNode* expNode);
FieldList Args(AstNode* argsNode);



// 错误输出
void printErrorInfo(int errorType, int lineNO, ...);

#endif