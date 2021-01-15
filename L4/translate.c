#include "translate.h"
#include "AstNode.h"
#include "semantic.h"
#include "type.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int var_no_cnt;
int label_no_cnt;
int tmp_no_cnt;

InterCodeNode intercodeListHead, intercodeListTail;
IC_Var icVarListHead, icVarListTail;

void addInterCodeNode(InterCodeNode newNode){
    if(newNode == NULL){
        return;
    }
    intercodeListTail->next = newNode;
    newNode->prev = intercodeListTail;
    while (intercodeListTail->next)
    {
        intercodeListTail = intercodeListTail->next;
    }
}

InterCode newInterCode(int kind){
    InterCode newCode = (InterCode)malloc(sizeof(struct InterCode_));
    newCode->kind = kind;
    return newCode;
}

InterCodeNode newInterCodeNode(InterCode newCode){
    InterCodeNode newNode = (InterCodeNode)malloc(sizeof(struct InterCodeNode_));
    newNode->code = newCode;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

InterCodeNode newInterCodeNodeByOp(Operand op, int kind){
    InterCode newCode = newInterCode(kind);
    newCode->u.x = op;
    InterCodeNode newNode = newInterCodeNode(newCode);
    return newNode;
}

Operand getIcVarOp(char* name){
    IC_Var currentIcVarPtr = icVarListHead->next;
    while (currentIcVarPtr)
    {
        if(!strcmp(currentIcVarPtr->name, name)){
            return currentIcVarPtr->op;
        }
        currentIcVarPtr = currentIcVarPtr->next;
    }
    Operand newOp = (Operand)malloc(sizeof(struct Operand_));
    newOp->kind = VARIABLE;
    newOp->u.var_no = var_no_cnt;
    var_no_cnt++;
    IC_Var newVar = (IC_Var)malloc(sizeof(struct IC_Var_));
    newVar->name = name;
    newVar->op = newOp;
    icVarListTail->next = newVar;
    icVarListTail = newVar;
    return newOp;
}

Operand constructConstant(int intValue){
    Operand newConstant = (Operand)malloc(sizeof(struct Operand_));
    newConstant->kind = CONSTANT;
    newConstant->u.value = intValue;
    return newConstant;
}

Operand constructTmp(){
    Operand newTmp = (Operand)malloc(sizeof(struct Operand_));
    newTmp->kind = TEMP;
    newTmp->u.tmp_no = tmp_no_cnt;
    tmp_no_cnt++;
    return newTmp;
}

Operand constructLabel(){
    Operand newLabel = (Operand)malloc(sizeof(struct Operand_));
    newLabel->kind = LABEL;
    newLabel->u.label_no = label_no_cnt;
    label_no_cnt++;
    return newLabel;
}

InterCodeNode joinCode(InterCodeNode first, InterCodeNode second){
    if(!first){
        return second;
    }
    InterCodeNode currentNodePtr = first;
    while(currentNodePtr->next){
        currentNodePtr = currentNodePtr->next;
    }
    currentNodePtr->next = second;
    if(second){
        second->prev = currentNodePtr;
    }
    return first;
}

void icInit(){
    var_no_cnt = 1;
    label_no_cnt = 1;
    tmp_no_cnt = 1;

    intercodeListHead = (InterCodeNode)malloc(sizeof(struct InterCodeNode_));
    intercodeListTail = intercodeListHead;

    icVarListHead = (IC_Var)malloc(sizeof(struct IC_Var_));
    icVarListTail = icVarListHead;
}

void icTranslate(AstNode* root, FILE* fp){
    icInit();
    trans_ExtDefList(root);
    InterCodeNode currentCodeNodePtr = intercodeListHead->next;
    while (currentCodeNodePtr)
    {
        fprintf(fp, "%s\n", getCodeStr(currentCodeNodePtr->code));
        currentCodeNodePtr = currentCodeNodePtr->next;
    }
}

void trans_ExtDefList(AstNode* extDefListNode){
    // ExtDef ExtDefList
    // \epsilon
    if(extDefListNode->lineNO != -1){
        addInterCodeNode(trans_ExtDef(extDefListNode->leftChild));
        trans_ExtDefList(extDefListNode->leftChild->rightSibling);
    }
}

InterCodeNode trans_ExtDef(AstNode* extDefNode){
    if(!extDefNode){
        return NULL;
    }
    AstNode* signNode = extDefNode->leftChild->rightSibling->rightSibling;
    if(signNode && !strcmp(signNode->value, COMP_ST)){
        InterCodeNode code1 = trans_FunDec(extDefNode->leftChild->rightSibling);
        InterCodeNode code2 = trans_CompSt(signNode);
        return joinCode(code1, code2);
    }
}

InterCodeNode trans_FunDec(AstNode* funDecNode){
    // ID LP RP
    // ID LP VarList RP
    InterCode newCode = newInterCode(IC_FUNC);
    char* funcName = funDecNode->leftChild->idValue;
    newCode->u.f = funcName;
    InterCodeNode funcCodeNode = newInterCodeNode(newCode);
    if(!strcmp(funDecNode->leftChild->rightSibling->rightSibling->value, VAR_LIST)){
        FieldList paramList = getFuncParamList(funcName);
        while (paramList)
        {
            InterCode paramCode = newInterCode(IC_PARAM);
            Operand op;
            if(paramList->type->kind == BASIC){
                op = getIcVarOp(paramList->name);
            } else {
                op = getIcVarOp(paramList->name);
                op->kind = ADDRESS;
                // op->u.tmp_no = tmp_no_cnt;
                // tmp_no_cnt++;
            }
            paramCode->u.x = op;
            InterCodeNode paramCodeNode = newInterCodeNode(paramCode);
            joinCode(funcCodeNode, paramCodeNode);
            paramList = paramList->tail;
        }
    }
    return funcCodeNode;
}

InterCodeNode trans_CompSt(AstNode* compStNode){
    // LC DefList StmtList RC
    InterCodeNode defListCodeNode = trans_DefList(compStNode->leftChild->rightSibling);
    InterCodeNode stmtListCodeNode = trans_StmtList(compStNode->leftChild->rightSibling->rightSibling);
    return joinCode(defListCodeNode, stmtListCodeNode);
}

InterCodeNode trans_StmtList(AstNode* stmtListNode){
    // Stmt StmtList
    // \epsilon
    if(stmtListNode->lineNO != -1){
        InterCodeNode stmtCodeNode = trans_Stmt(stmtListNode->leftChild);
        InterCodeNode stmtListCodeNode = trans_StmtList(stmtListNode->leftChild->rightSibling);
        return joinCode(stmtCodeNode, stmtListCodeNode);
    }
    return NULL;
}

InterCodeNode trans_Stmt(AstNode* stmtNode){
    if(!stmtNode){
        return NULL;
    }
    // P83
    if(!strcmp(stmtNode->leftChild->value, EXP)){
        // Exp SEMI
        return trans_Exp(stmtNode->leftChild, NULL);
    } else if(!strcmp(stmtNode->leftChild->value, COMP_ST)){
        // CompSt
        return trans_CompSt(stmtNode->leftChild);
    } else if(!strcmp(stmtNode->leftChild->value, TER_RETURN)){
        // RETURN Exp SEMI
        Operand t1 = constructTmp();
        InterCodeNode code1 = trans_Exp(stmtNode->leftChild->rightSibling, t1);
        InterCodeNode code2 = newInterCodeNodeByOp(t1, IC_RETURN);
        return joinCode(code1, code2);
    } else if(!strcmp(stmtNode->leftChild->value, TER_IF)){
        // IF LP Exp RP Stmt1
        // IF LP Exp RP Stmt1 ELSE Stmt2
        AstNode* expNode = stmtNode->leftChild->rightSibling->rightSibling;
        AstNode* stmt1Node = expNode->rightSibling->rightSibling;
        Operand label1 = constructLabel();
        Operand label2 = constructLabel();
        InterCodeNode code1 = trans_Cond(expNode, label1, label2);
        InterCodeNode code2 = trans_Stmt(stmt1Node);
        InterCodeNode label1CodeNode = newInterCodeNodeByOp(label1, IC_LABEL);
        InterCodeNode label2CodeNode = newInterCodeNodeByOp(label2, IC_LABEL);
        if(!(stmt1Node->rightSibling)){
            // IF LP Exp RP Stmt1
            return joinCode(joinCode(joinCode(code1, label1CodeNode), code2), label2CodeNode);
        } else {
            // IF LP Exp RP Stmt1 ELSE Stmt2
            Operand label3 = constructLabel();
            InterCodeNode code3 = trans_Stmt(stmt1Node->rightSibling->rightSibling);
            InterCodeNode label3CodeNode = newInterCodeNodeByOp(label3, IC_LABEL);
            InterCodeNode goto3CodeNode = newInterCodeNodeByOp(label3, IC_GOTO);
            return joinCode(joinCode(joinCode(joinCode(joinCode(joinCode(code1, label1CodeNode), code2), goto3CodeNode), label2CodeNode), code3), label3CodeNode);
        }
    } else if(!strcmp(stmtNode->leftChild->value, TER_WHILE)){
        // WHILE LP Exp RP Stmt
        Operand label1 = constructLabel();
        Operand label2 = constructLabel();
        Operand label3 = constructLabel();
        AstNode* expNode = stmtNode->leftChild->rightSibling->rightSibling;
        InterCodeNode code1 = trans_Cond(expNode, label2, label3);
        InterCodeNode code2 = trans_Stmt(expNode->rightSibling->rightSibling);
        InterCodeNode label1CodeNode = newInterCodeNodeByOp(label1, IC_LABEL);
        InterCodeNode label2CodeNode = newInterCodeNodeByOp(label2, IC_LABEL);
        InterCodeNode label3CodeNode = newInterCodeNodeByOp(label3, IC_LABEL);
        InterCodeNode goto1CodeNode = newInterCodeNodeByOp(label1, IC_GOTO);
        return joinCode(joinCode(joinCode(joinCode(joinCode(label1CodeNode, code1), label2CodeNode), code2), goto1CodeNode), label3CodeNode);
    }
}

InterCodeNode trans_DefList(AstNode* defListNode){
    InterCodeNode code = NULL;
    while (defListNode->lineNO != -1)
    {
        AstNode* defNode = defListNode->leftChild;
        defListNode = defListNode->leftChild->rightSibling;
        AstNode* decListNode = defNode->leftChild->rightSibling;
        AstNode* decNode = decListNode->leftChild;
        code = joinCode(code, trans_Dec(decNode));
        while (decNode->rightSibling)
        {
            decListNode = decNode->rightSibling->rightSibling;
            decNode = decListNode->leftChild;
            code = joinCode(code, trans_Dec(decNode));
        }
    }
    return code;
}

InterCodeNode trans_Dec(AstNode* decNode){
    if(!decNode){
        return NULL;
    }
    AstNode* varDecNode = decNode->leftChild;
    if(decNode->leftChild->rightSibling){
        // VarDec ASSIGNOP Exp
        if(!strcmp(varDecNode->leftChild->value, TER_ID)){
            Operand op = getIcVarOp(varDecNode->leftChild->idValue);
            return trans_Exp(decNode->leftChild->rightSibling->rightSibling, op);
        }
    } else{
        // VarDec
        if(!strcmp(varDecNode->leftChild->value, VAR_DEC)){
            // VarDec LB INT RB
            // 最高一维数组
            Operand variable = getIcVarOp(varDecNode->leftChild->leftChild->idValue);
            variable->kind = ARRAY_STRUCT;
            InterCode intercode = newInterCode(IC_DEC);
            intercode->u.dec.x = variable;
            intercode->u.dec.size = varDecNode->leftChild->rightSibling->rightSibling->intValue * 4;
            return newInterCodeNode(intercode);
        }
    }
    return NULL;
}

InterCodeNode trans_Cond(AstNode* expNode, Operand label_true, Operand label_false){
    if(expNode->leftChild->rightSibling){
        if(!strcmp(expNode->leftChild->rightSibling->value, TER_RELOP)){
            // Exp RELOP Exp
            Operand t1 = constructTmp();
            Operand t2 = constructTmp();
            InterCodeNode code1 = trans_Exp(expNode->leftChild, t1);
            InterCodeNode code2 = trans_Exp(expNode->leftChild->rightSibling->rightSibling, t2);
            InterCode intercode = newInterCode(IC_IF);
            intercode->u.if_state.x = t1;
            intercode->u.if_state.relop = expNode->leftChild->rightSibling->idValue;
            intercode->u.if_state.y = t2;
            intercode->u.if_state.z = label_true;
            InterCodeNode code3 = newInterCodeNode(intercode);
            InterCodeNode code4 = newInterCodeNodeByOp(label_false, IC_GOTO);
            return joinCode(joinCode(joinCode(code1, code2), code3), code4);
        } else if(!strcmp(expNode->leftChild->value, TER_NOT)){
            // NOT Exp
            return trans_Cond(expNode->leftChild->rightSibling, label_false, label_true);
        } else if(!strcmp(expNode->leftChild->rightSibling->value, TER_AND)){
            // Exp AND Exp
            Operand label1 = constructLabel();
            InterCodeNode code1 = trans_Cond(expNode->leftChild, label1, label_false);
            InterCodeNode code2 = trans_Cond(expNode->leftChild->rightSibling->rightSibling, label_true, label_false);
            return joinCode(joinCode(code1, newInterCodeNodeByOp(label1, IC_LABEL)), code2);
        } else if(!strcmp(expNode->leftChild->rightSibling->value, TER_OR)){
            Operand label1 = constructLabel();
            InterCodeNode code1 = trans_Cond(expNode->leftChild, label_true, label1);
            InterCodeNode code2 = trans_Cond(expNode->leftChild->rightSibling->rightSibling, label_true, label_false);
            return joinCode(joinCode(code1, newInterCodeNodeByOp(label1, IC_LABEL)), code2);
        }
    }
    // other cases
    Operand t1 = constructTmp();
    InterCodeNode code1 = trans_Exp(expNode, t1);
    InterCode intercode = newInterCode(IC_IF);
    intercode->u.if_state.x = t1;
    intercode->u.if_state.relop = "!=";
    intercode->u.if_state.y = constructConstant(0);
    intercode->u.if_state.z = label_true;
    InterCodeNode code2 = newInterCodeNode(intercode);
    return joinCode(joinCode(code1, code2), newInterCodeNodeByOp(label_false, IC_GOTO));
}

InterCodeNode trans_Exp(AstNode* expNode, Operand place){
    if(expNode == NULL){
        return NULL;
    }
    if(expNode->leftChild == NULL){
        return NULL;
    }
    // P82
    char* value = expNode->leftChild->value;
    if(!strcmp(value, TER_INT)){
        // INT
        InterCode newCode = newInterCode(IC_ASSIGN);
        newCode->u.assign.left = place;
        newCode->u.assign.right = constructConstant(expNode->leftChild->intValue);
        return newInterCodeNode(newCode);
    } else if(!strcmp(value, TER_ID)){
        char* idValue = expNode->leftChild->idValue;
        if(!(expNode->leftChild->rightSibling)){
            // ID
            Operand op = getIcVarOp(idValue);
            if(op->kind == ARRAY_STRUCT){
                InterCode newCode = newInterCode(IC_REFER);
                newCode->u.assign.left = place;
                newCode->u.assign.right = op;
                return newInterCodeNode(newCode);
            } else {
                InterCode newCode = newInterCode(IC_ASSIGN);
                newCode->u.assign.left = place;
                newCode->u.assign.right = op;
                return newInterCodeNode(newCode);
            }
        } else if(!strcmp(expNode->leftChild->rightSibling->rightSibling->value, ARGS)){
            // P85
            // ID LP Args RP
            char* funcName = idValue;
            FieldList paramList = getFuncParamList(funcName);
            IC_Arg realParamList = NULL;
            InterCodeNode paramCodeNode = trans_Args(expNode->leftChild->rightSibling->rightSibling, &realParamList);
            if(!strcmp(funcName, "write")){
                InterCode funcCode = newInterCode(IC_WRITE);
                funcCode->u.x = realParamList->op;
                return joinCode(paramCodeNode, newInterCodeNode(funcCode));
            } else {
                InterCodeNode realParamCodeNode = NULL;
                while (realParamList)
                {
                    InterCode realParamCode = newInterCode(IC_ARG);
                    realParamCode->u.x = realParamList->op;
                    realParamCodeNode = joinCode(realParamCodeNode, newInterCodeNode(realParamCode));
                    realParamList = realParamList->next;
                }
                InterCode callCode = newInterCode(IC_CALL);
                if(place){
                    callCode->u.call.x = place;
                } else
                {
                    callCode->u.call.x = constructTmp();
                }
                callCode->u.call.f = funcName;
                InterCodeNode callCodeNode = newInterCodeNode(callCode);
                return joinCode(joinCode(paramCodeNode, realParamCodeNode), callCodeNode);
            }
        } else{
            // ID LP RP
            char* funcName = idValue;
            if(!strcmp(funcName, "read")){
                InterCode funcCode = newInterCode(IC_READ);
                funcCode->u.x = place;
                return newInterCodeNode(funcCode);
            } else {
                InterCode callCode = newInterCode(IC_CALL);
                if(place){
                    callCode->u.call.x = place;
                } else
                {
                    callCode->u.call.x = constructTmp();
                }
                callCode->u.call.f = funcName;
                return newInterCodeNode(callCode);
            }
        }
    } else if(expNode->leftChild->rightSibling && !strcmp(expNode->leftChild->rightSibling->value, TER_ASSIGNOP)){
        // Exp ASSIGN Exp
        if(!strcmp(expNode->leftChild->leftChild->value, TER_ID)){
            // Exp_1 \to ID
            Operand variable = getIcVarOp(expNode->leftChild->leftChild->idValue);
            Operand t1 = constructTmp();
            InterCodeNode code1 = trans_Exp(expNode->leftChild->rightSibling->rightSibling, t1);
            InterCode assignCode = newInterCode(IC_ASSIGN);
            assignCode->u.assign.left = variable;
            assignCode->u.assign.right = t1;
            InterCodeNode code2 = newInterCodeNode(assignCode);
            if(place){
                InterCode assignCode2 = newInterCode(IC_ASSIGN);
                assignCode2->u.assign.left = place;
                assignCode2->u.assign.right = variable;
                code2 = joinCode(code2, newInterCodeNode(assignCode2));
            }
            return joinCode(code1, code2);
        } else {
            Operand variable = constructTmp();
            variable->kind = ADDRESS;
            InterCodeNode code1 = trans_Exp(expNode->leftChild, variable);
            Operand t1 = constructTmp();
            InterCodeNode code2 = trans_Exp(expNode->leftChild->rightSibling->rightSibling, t1);
            InterCode addrAssignCode = newInterCode(IC_ADDR_ASSIGN);
            addrAssignCode->u.assign.left = variable;
            addrAssignCode->u.assign.right = t1;
            InterCodeNode code3 = newInterCodeNode(addrAssignCode);
            if(place){
                InterCode assignCode = newInterCode(IC_ASSIGN);
                assignCode->u.assign.left = place;
                assignCode->u.assign.right = variable;
                code3 = joinCode(code3, newInterCodeNode(assignCode));
            }
            return joinCode(joinCode(code1, code2), code3);
        }
    } else if(expNode->leftChild->rightSibling && 
                (!strcmp(expNode->leftChild->rightSibling->value, TER_PLUS)
                || !strcmp(expNode->leftChild->rightSibling->value, TER_MINUS)
                || !strcmp(expNode->leftChild->rightSibling->value, TER_STAR)
                || !strcmp(expNode->leftChild->rightSibling->value, TER_DIV))){
        // Exp PLUS Exp
        // Exp MINUS Exp
        // Exp STAR Exp
        // Exp DIV Exp
        Operand t1 = constructTmp();
        Operand t2 = constructTmp();
        InterCodeNode code1 = trans_Exp(expNode->leftChild, t1);
        InterCodeNode code2 = trans_Exp(expNode->leftChild->rightSibling->rightSibling, t2);
        InterCode intercode = (InterCode)malloc(sizeof(struct InterCode_));
        if(!strcmp(expNode->leftChild->rightSibling->value, TER_PLUS)){
            intercode->kind = IC_ADD;
        } else if(!strcmp(expNode->leftChild->rightSibling->value, TER_MINUS)){
            intercode->kind = IC_SUB;
        } else if(!strcmp(expNode->leftChild->rightSibling->value, TER_STAR)){
            intercode->kind = IC_MUL;
        } else if(!strcmp(expNode->leftChild->rightSibling->value, TER_DIV)){
            intercode->kind = IC_DIV;
        }
        intercode->u.binop.op1 = t1;
        intercode->u.binop.op2 = t2;
        intercode->u.binop.result = place;
        InterCodeNode code3 = newInterCodeNode(intercode);
        return joinCode(joinCode(code1, code2), code3);
    } else if(!strcmp(expNode->leftChild->value, TER_MINUS)){
        // MINUS Exp
        Operand t1 = constructTmp();
        InterCodeNode code1 = trans_Exp(expNode->leftChild->rightSibling, t1);
        InterCode intercode = newInterCode(IC_SUB);
        intercode->u.binop.op1 = constructConstant(0);
        intercode->u.binop.op2 = t1;
        intercode->u.binop.result = place;
        InterCodeNode code2 = newInterCodeNode(intercode);
        return joinCode(code1, code2);
    } else if(expNode->leftChild->rightSibling && 
                (!strcmp(expNode->leftChild->rightSibling->value, TER_RELOP)
                || !strcmp(expNode->leftChild->rightSibling->value, TER_AND)
                || !strcmp(expNode->leftChild->rightSibling->value, TER_OR)
                || !strcmp(expNode->leftChild->value, TER_NOT))){
        // Exp RELOP Exp
        // NOT Exp
        // Exp AND Exp
        // Exp OR Exp
        Operand label1 = constructLabel();
        Operand label2 = constructLabel();
        InterCode intercode0 = newInterCode(IC_ASSIGN);
        intercode0->u.assign.left = place;
        intercode0->u.assign.right = constructConstant(0);
        InterCodeNode code0 = newInterCodeNode(intercode0);
        InterCodeNode code1 = trans_Cond(expNode, label1, label2);
        InterCodeNode code2 = newInterCodeNodeByOp(label1, IC_LABEL);
        InterCode intercode1 = newInterCode(IC_ASSIGN);
        intercode1->u.assign.left = place;
        intercode1->u.assign.right = constructConstant(1);
        code2 = joinCode(code2, newInterCodeNode(intercode1));
        return joinCode(joinCode(joinCode(code0, code1), code2), newInterCodeNodeByOp(label2, IC_LABEL));
    } else if(expNode->leftChild->rightSibling && !strcmp(expNode->leftChild->rightSibling->value, TER_DOT)){

    } else if(expNode->leftChild->rightSibling && !strcmp(expNode->leftChild->rightSibling->value, TER_LB)){
        // Exp LB Exp RB
        // 没有高维数组
        Operand v1 = getIcVarOp(expNode->leftChild->leftChild->idValue);
        Operand base = constructTmp();
        base->kind = ADDRESS;
        InterCode intercode = NULL;
        if(v1->kind == ADDRESS){
            intercode = newInterCode(IC_ASSIGN);
        } else {
            intercode = newInterCode(IC_REFER);
        }
        intercode->u.assign.left = base;
        intercode->u.assign.right = v1;
        InterCodeNode code1 = newInterCodeNode(intercode);
        Operand t1_offset = constructTmp();
        Operand t2_final_addr = constructTmp();
        t2_final_addr->kind = ADDRESS;
        InterCodeNode code2 = trans_Exp(expNode->leftChild->rightSibling->rightSibling, t1_offset);
        InterCode intercode2 = newInterCode(IC_MUL);
        intercode2->u.binop.op1 = t1_offset;
        intercode2->u.binop.op2 = constructConstant(4);
        intercode2->u.binop.result = t1_offset;
        InterCodeNode code3 = newInterCodeNode(intercode2);
        InterCodeNode code4 = NULL;
        if(place){
            InterCode intercode3 = newInterCode(IC_ADD);
            intercode3->u.binop.op1 = base;
            intercode3->u.binop.op2 = t1_offset;
            intercode3->u.binop.result = t2_final_addr;
            code4 = newInterCodeNode(intercode3);
            InterCode intercode4 = newInterCode(IC_ASSIGN);
            intercode4->u.assign.left = place;
            intercode4->u.assign.right = t2_final_addr;
            code4 = joinCode(code4, newInterCodeNode(intercode4));
        }
        return joinCode(joinCode(joinCode(code1, code2), code3), code4);
    } else if(!strcmp(expNode->leftChild->value, TER_LP)){
        // LP Exp Rp
        return trans_Exp(expNode->leftChild->rightSibling, place);
    }
}

InterCodeNode trans_Args(AstNode* argsNode, IC_Arg* argList){
    Operand t1 = constructTmp();
    InterCodeNode code1 = trans_Exp(argsNode->leftChild, t1);
    IC_Arg newArg = (IC_Arg)malloc(sizeof(struct IC_Arg_));
    newArg->op = t1;
    newArg->next = *argList;
    *argList = newArg;
    if(!(argsNode->leftChild->rightSibling)){
        // Exp
        return code1;
    } else {
        // Exp COMMA Args
        InterCodeNode code2 = trans_Args(argsNode->leftChild->rightSibling->rightSibling, argList);
        return joinCode(code1, code2);
    }
}

char* getOpStr(Operand op){
    char* opStr = (char*)malloc(64);
    switch (op->kind)
    {
        case VARIABLE:
            sprintf(opStr, "v%d", op->u.var_no);
            break;
        case CONSTANT:
            sprintf(opStr, "#%d", op->u.value);
            break;
        case ADDRESS:
            sprintf(opStr, "t%d", op->u.tmp_no);
            break;
        case LABEL:
            sprintf(opStr, "label%d", op->u.label_no);
            break;
        case ARRAY_STRUCT:
            sprintf(opStr, "v%d", op->u.var_no);
            break;
        case TEMP:
            sprintf(opStr, "t%d", op->u.tmp_no);
            break;
        default:
            break;
    }
    return opStr;
}

char* getCodeStr(InterCode code){
    char* codeStr = (char*)malloc(100);
    memset(codeStr, 0, 100);
    char *x, *y, *z, *f;
    Operand left, right;
    switch (code->kind)
    {
        case IC_LABEL:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "LABEL %s :", x);
            break;
        case IC_FUNC:
            f = code->u.f;
            sprintf(codeStr, "FUNCTION %s :", f);
            break;
        case IC_ASSIGN:
            left = code->u.assign.left;
            right = code->u.assign.right;
            if(code->u.assign.left){
                x = getOpStr(left);
                y = getOpStr(right);
                if(left->kind == ADDRESS && right->kind != ADDRESS){
                    sprintf(codeStr, "%s := &%s", x, y);
                } else if (left->kind != ADDRESS && right->kind == ADDRESS){
                    sprintf(codeStr, "%s := *%s", x, y);
                } else {
                    sprintf(codeStr, "%s := %s", x, y);
                }
            }
            break;
        case IC_ADD:
            if(code->u.binop.result){
                x = getOpStr(code->u.binop.result);
                y = getOpStr(code->u.binop.op1);
                z = getOpStr(code->u.binop.op2);
                sprintf(codeStr, "%s := %s + %s", x, y, z);
            }
            break;
        case IC_SUB:
            if(code->u.binop.result != NULL){
                x = getOpStr(code->u.binop.result);
                y = getOpStr(code->u.binop.op1);
                z = getOpStr(code->u.binop.op2);
                sprintf(codeStr, "%s := %s - %s", x, y, z);
            }
            break;
        case IC_MUL:
            if(code->u.binop.result != NULL){
                x = getOpStr(code->u.binop.result);
                y = getOpStr(code->u.binop.op1);
                z = getOpStr(code->u.binop.op2);
                sprintf(codeStr, "%s := %s * %s", x, y, z);
            }
            break;
        case IC_DIV:
            if(code->u.binop.result != NULL){
                x = getOpStr(code->u.binop.result);
                y = getOpStr(code->u.binop.op1);
                z = getOpStr(code->u.binop.op2);
                sprintf(codeStr, "%s := %s / %s", x, y, z);
            }
            break;
        case IC_REFER:
            x = getOpStr(code->u.assign.left);
            y = getOpStr(code->u.assign.right);
            sprintf(codeStr, "%s := &%s", x, y);
            break;
        case IC_DE_REFER:
            x = getOpStr(code->u.assign.left);
            y = getOpStr(code->u.assign.right);
            sprintf(codeStr, "%s := *%s", x, y);
            break;
        case IC_ADDR_ASSIGN:
            if(code->u.assign.left){
                x = getOpStr(code->u.assign.left);
                y = getOpStr(code->u.assign.right);
                sprintf(codeStr, "*%s := %s", x, y);
            }
            break;
        case IC_GOTO:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "GOTO %s", x);
            break;
        case IC_IF:
            x = getOpStr(code->u.if_state.x);
            y = getOpStr(code->u.if_state.y);
            z = getOpStr(code->u.if_state.z);
            sprintf(codeStr, "IF %s %s %s GOTO %s", x, code->u.if_state.relop, y, z);
            break;
        case IC_RETURN:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "RETURN %s", x);
            break;
        case IC_DEC:
            x = getOpStr(code->u.dec.x);
            sprintf(codeStr, "DEC %s %d", x, code->u.dec.size);
            break;
        case IC_ARG:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "ARG %s", x);
            break;
        case IC_CALL:
            x = getOpStr(code->u.call.x);
            f = code->u.call.f;
            sprintf(codeStr, "%s := CALL %s", x, f);
            break;
        case IC_PARAM:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "PARAM %s", x);
            break;
        case IC_READ:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "READ %s", x);
            break;
        case IC_WRITE:
            x = getOpStr(code->u.x);
            sprintf(codeStr, "WRITE %s", x);
            break;
        default:
            break;
    }
    return codeStr;
}
