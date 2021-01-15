#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "AstNode.h"
#include<stdio.h>

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct InterCodeNode_* InterCodeNode;
typedef struct IC_Var_* IC_Var;
typedef struct IC_Arg_* IC_Arg;

struct Operand_
{
    enum { VARIABLE, CONSTANT, ADDRESS, LABEL, ARRAY_STRUCT, TEMP } kind;
    union
    {
        int var_no;
        int value;
        int label_no;
        int tmp_no;
    } u;
};

struct InterCode_
{
    enum { 
        IC_LABEL, 
        IC_FUNC,
        IC_ASSIGN, 
        IC_ADD, 
        IC_SUB, 
        IC_MUL, 
        IC_DIV,
        IC_REFER,
        IC_DE_REFER,
        IC_ADDR_ASSIGN,
        IC_GOTO,
        IC_IF,
        IC_RETURN,
        IC_DEC,
        IC_ARG,
        IC_CALL,
        IC_PARAM,
        IC_READ,
        IC_WRITE
    } kind;
    union
    {
        char* f;
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
        struct { Operand x, y, z; char* relop; } if_state;
        struct { Operand x; int size; } dec;
        struct { Operand x; char* f; } call;
        Operand x;
    } u;
    
};

struct InterCodeNode_
{
    InterCode code;
    InterCodeNode prev, next;
};

struct IC_Var_{
    char* name;
    Operand op;
    IC_Var next;
};

struct IC_Arg_
{
    Operand op;
    IC_Arg next;
};

void addInterCodeNode(InterCodeNode newNode);
InterCode newInterCode(int kind);
InterCodeNode newInterCodeNode(InterCode newCode);
InterCodeNode newInterCodeNodeByOp(Operand op, int kind);
Operand getIcVarOp(char* name);
Operand constructConstant(int intValue);
Operand constructTmp();
Operand constructLabel();
InterCodeNode joinCode(InterCodeNode first, InterCodeNode second);

void icInit();
void icTranslate(AstNode* root, FILE* fp);
void trans_ExtDefList(AstNode* extDefListNode);

InterCodeNode trans_ExtDef(AstNode* extDefNode);
InterCodeNode trans_FunDec(AstNode* funDecNode);
InterCodeNode trans_CompSt(AstNode* compStNode);
InterCodeNode trans_StmtList(AstNode* stmtListNode);
InterCodeNode trans_Stmt(AstNode* stmtNode);
InterCodeNode trans_DefList(AstNode* defListNode);
InterCodeNode trans_Dec(AstNode* decNode);
InterCodeNode trans_Cond(AstNode* expNode, Operand label_true, Operand label_false);
InterCodeNode trans_Exp(AstNode* expNode, Operand place);
InterCodeNode trans_Args(AstNode* argsNode, IC_Arg* argList);

char* getOpStr(Operand op);
char* getCodeStr(InterCode code);

#endif