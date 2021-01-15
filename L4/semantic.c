#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
#include"semantic.h"
#include"AstNode.h"
#include"type.h"

// High-level Definitions
char* EXT_DEF_LIST = "ExtDefList";
char* EXT_DEF = "ExtDef";
char* EXT_DEC_LIST = "ExtDecList";

// Specifiers
char* SPECIFIER = "Specifier";
char* STRUCT_SPECIFIER = "StructSpecifier";
char* OPT_TAG = "OptTag";
char* TAG = "Tag";

// Declarators
char* VAR_DEC = "VarDec";
char* FUN_DEC = "FunDec";
char* VAR_LIST = "VarList";
char* PARAM_DEC = "ParamDec";

// Statements
char* COMP_ST = "CompSt";
char* STMT_LIST = "StmtList";
char* STMT = "Stmt";

// Local Definitions
char* DEF_LIST = "DefList";
char* DEF = "Def";
char* DEC_LIST = "DecList";
char* DEC = "Dec";

// Expressions
char* EXP = "Exp";
char* ARGS = "Args";

// Tokens
char* TER_SEMI = "SEMI";
char* TER_ID = "ID";
char* TER_TYPE = "TYPE";
char* TER_ASSIGNOP = "ASSIGNOP";
char* TER_AND = "AND";
char* TER_OR = "OR";
char* TER_RELOP = "RELOP";
char* TER_PLUS = "PLUS";
char* TER_MINUS = "MINUS";
char* TER_STAR = "STAR";
char* TER_DIV = "DIV";
char* TER_LP = "LP";
char* TER_RP = "RP";
char* TER_LB = "LB";
char* TER_RB = "RB";
char* TER_NOT = "NOT";
char* TER_DOT = "DOT";
char* TER_INT = "INT";
char* TER_FLOAT = "FLOAT";
char* TER_RETURN = "RETURN";
char* TER_IF = "IF";
char* TER_ELSE = "ELSE";
char* TER_WHILE = "WHILE";

void ExtDefList(AstNode* extDefListNode){
    // if(!extDefListNode){
    //     return;
    // }
    if(extDefListNode->leftChild && extDefListNode->leftChild->lineNO != -1){
        // ExtDef ExtDefList
        ExtDef(extDefListNode->leftChild);
        ExtDefList(extDefListNode->leftChild->rightSibling);
    }
}

void ExtDef(AstNode* extDefNode){
    // if(!extDefNode){
    //     return;
    // }
    AstNode* signNode = extDefNode->leftChild->rightSibling;
    Type type = Specifier(extDefNode->leftChild);
    if(!strcmp(signNode->value, EXT_DEC_LIST)){
        // Specifier ExtDecList SEMI
        ExtDecList(signNode, type);
    } else if(!strcmp(signNode->value, TER_SEMI)){
        // Specifier SEMI
    } else if(!strcmp(signNode->value, FUN_DEC)){
        // Specifier FunDec CompSt
        Function func = FunDec(signNode, type);
        if(!func){
            // func中出错（重定义）
            return;
        }
        CompSt(signNode->rightSibling, type);
    }
}

void ExtDecList(AstNode* extDecListNode, Type type){
    // if(!extDecListNode){
    //     return;
    // }
    AstNode* varDecNode = extDecListNode->leftChild;
    VarDec(varDecNode, type, 0);
    if(varDecNode->rightSibling){
        ExtDecList(varDecNode->rightSibling->rightSibling, type);
    }
}

Type Specifier(AstNode* specifierNode){
    if(!specifierNode){
        return NULL;
    }
    AstNode* child = specifierNode->leftChild;
    Type type = (Type)malloc(sizeof(struct Type_));
    if(!strcmp(child->value, TER_TYPE)){
        // TYPE
        type->kind = BASIC;
        if(!strcmp(child->typeValue, INT_TYPE_NAME)){
            type->u.basic = INT_TYPE;
        } else if(!strcmp(child->typeValue, FLOAT_TYPE_NAME)){
            type->u.basic = FLOAT_TYPE;
        }
    } else if(!strcmp(child->value, STRUCT_SPECIFIER)){
        // StructSpecifier
        type = StructSpecifier(child);
    }
    return type;
}

Type StructSpecifier(AstNode* structSpecifierNode){
    if(!structSpecifierNode){
        return NULL;
    }
    AstNode* signNode = structSpecifierNode->leftChild->rightSibling;
    if(!strcmp(signNode->value, OPT_TAG)){
        int lineNO = signNode->lineNO;

        FieldList backupStruct = (FieldList)malloc(sizeof(struct FieldList_));
        // fprintf(stderr, "line140: %d\n", lineNO);
        // printCurrentStructField();
        backupStruct = backupCurrentStruct(backupStruct);
        // if(!backupStruct){
        //     fprintf(stderr, "null?\n");
        // }
        // backupCurrentStruct(backupStruct);
        initCurrentStruct();

        FieldList fieldList = DefList(signNode->rightSibling->rightSibling, 1);

        recoverCurrentStruct(backupStruct);

        Type type = (Type)malloc(sizeof(struct Type_));
        type->kind = STRUCTURE;
        type->u.structure = fieldList;
        // 注意这里把结构体当作左值了
        type->prop = LEFT_VALUE;  

        if(lineNO != -1){
            // OptTag ——> ID
            char* structName = signNode->leftChild->idValue;
            if(isStructExist(structName) || isVarExist(structName)){
                // 错误类型16：结构体的名字与前面定义过的结构体或变量的名字重复
                printErrorInfo(STURCT_REDEFINED, lineNO, structName);
                return NULL;
            } else {
                addStruct(structName, fieldList);
                return type;
            }
        } else {
            // OptTag ——> \epsilon
            char* structName = NULL;
            addStruct(structName, fieldList);
            return type;
        }
    } else if(!strcmp(signNode->value, TAG)){
        // Struct Tag
        // Tag ——> ID
        char* structName = signNode->leftChild->idValue;
        if(!isStructExist(structName)){
            printErrorInfo(STRUCT_NOT_DEFINED, signNode->lineNO, structName); 
        }
        return getStructType(structName);
    }
}

/**
 * 1. 变量检查是否已经定义，直接加入变量表，封装成FieldList返回
 * 2. 结构体检查是否已经定义，不加入结构体表，封装成FieldList返回
 * 3. 结构体域检查是否已经定义，如果没有，该域中增加定义(TODO)
 */
FieldList VarDec(AstNode* varDecNode, Type type, int isForStruct){
    if(!varDecNode){
        return NULL;
    }
    if(!strcmp(varDecNode->leftChild->value, TER_ID)){
        // ID
        char* name = varDecNode->leftChild->idValue;
        int lineNO = varDecNode->leftChild->lineNO;
        if(!isForStruct){
            if(isVarExist(name) || isStructExist(name)){
                printErrorInfo(VAR_REDEFINED, lineNO, name);
            } else {
                addVar(name, type);
            }
        } else{
            if(isVarInCurrentStruct(name)){
                // printCurrentStructField();
                printErrorInfo(ERROR_STRUCT_FIELD, lineNO, name);
            } else{
                addVarInCurrentStruct(name, type);
            }
        }
        
        FieldList fieldList = (FieldList)malloc(sizeof(struct FieldList_));
        fieldList->name = name;
        fieldList->type = type;
        fieldList->tail = NULL;
        return fieldList;
    } else if(!strcmp(varDecNode->leftChild->value, VAR_DEC)){
        // VarDec LB INT RB
        Type trueType = (Type)malloc(sizeof(struct Type_));
        trueType->kind = ARRAY;
        trueType->u.array.elem = type;
        trueType->u.array.size = varDecNode->leftChild->rightSibling->rightSibling->intValue;
        return VarDec(varDecNode->leftChild, trueType, isForStruct);
    }
    
}

Function FunDec(AstNode* funDecNode, Type type){
    if(!funDecNode){
        return NULL;
    }
    // ID LP (VarList) RP
    char* funcName = funDecNode->leftChild->idValue;
    // fprintf(stderr, "here func name: %s\n", funcName);
    // printSignalList();
    if(isFuncExist(funcName)){
        printErrorInfo(FUNC_REDEFINED, funDecNode->lineNO, funcName);
        return NULL;
    }
    Function func = (Function)malloc(sizeof(struct Function_));
    func->name = funcName;
    func->line = funDecNode->lineNO;
    func->type = type;
    if(!(strcmp(funDecNode->leftChild->rightSibling->rightSibling->value, VAR_LIST))){
        FieldList paramField = VarList(funDecNode->leftChild->rightSibling->rightSibling);
        func->parameter = paramField;
    } else {
        func->parameter = NULL;
    }
    addFunc(func->name, func->line, func->type, func->parameter);
    return func;
}

FieldList VarList(AstNode* varListNode){
    if(!varListNode){
        return NULL;
    }
    FieldList paramField = ParamDec(varListNode->leftChild);
    if(varListNode->leftChild->rightSibling){ 
        // ParamDec COMMA VarList
        FieldList varField = VarList(varListNode->leftChild->rightSibling->rightSibling);
        if(!paramField){
            return varField;
        }else{
            paramField->tail = varField;
        }
    }
    return paramField;
}

FieldList ParamDec(AstNode* paramDecNode){
    if(!paramDecNode){
        return NULL;
    }
    // Specifier VarDec
    Type type = Specifier(paramDecNode->leftChild);
    return VarDec(paramDecNode->leftChild->rightSibling, type, 0);
}

void CompSt(AstNode* compStNode, Type type){
    // if(!compStNode){
    //     return NULL;
    // }
    // LC DefList StmtList RC
    DefList(compStNode->leftChild->rightSibling, 0);
    StmtList(compStNode->leftChild->rightSibling->rightSibling, type);
}

void StmtList(AstNode* stmtListNode, Type type){
    // if(!stmtListNode){
    //     return;
    // }
    if(stmtListNode->leftChild && stmtListNode->leftChild->lineNO != -1){
        // Stmt StmtList
        Stmt(stmtListNode->leftChild, type);
        StmtList(stmtListNode->leftChild->rightSibling, type);
    }
    // \epsilon
}

void Stmt(AstNode* stmtNode, Type type){
    // if(!stmtNode){
    //     return;
    // }
    AstNode* child = stmtNode->leftChild;
    if(!strcmp(child->value, EXP)){ 
        // Exp SEMI
        Exp(child);
    } else if(!strcmp(child->value, COMP_ST)){ 
        // CompSt
        CompSt(child, type);
    } else if(!strcmp(child->value, TER_RETURN)){ 
        // RETURN Exp SEMI
        Type typeExp = Exp(child->rightSibling);
		if(!typeExp){
            return;
        }
        if(!isTypeMatched(type, typeExp)){
            printErrorInfo(RETURN_UNMATCHED, child->lineNO);
        }
    } else if(!strcmp(child->value, TER_IF)){ 
        Exp(child->rightSibling->rightSibling);
        Stmt(child->rightSibling->rightSibling->rightSibling->rightSibling, type);
        if(child->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling){
            // IF LP Exp RP Stmt ELSE Stmt
            Stmt(child->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, type);
        }
    }else if(!strcmp(child->value, TER_WHILE)){ 
        // WHILE LP Exp RP Stmt
        Exp(child->rightSibling->rightSibling);
        Stmt(child->rightSibling->rightSibling->rightSibling->rightSibling, type);
    }
}

FieldList DefList(AstNode* defListNode, int isForStruct){
    if(!defListNode){
        return NULL;
    }
    if(defListNode->lineNO == -1){
        // \epsilon
        return NULL;
    } else{
        // Def DefList
        if(!(defListNode->leftChild)){
            return NULL;
        }
        FieldList defField = Def(defListNode->leftChild, isForStruct);
        FieldList defListField = DefList(defListNode->leftChild->rightSibling, isForStruct);
        if(!defField){
            return defListField;
        }
        FieldList currentPtr = defField;
        while (currentPtr->tail)
        {
            currentPtr = currentPtr->tail;
        }
        currentPtr->tail = defListField;
        return defField;

    }
}

FieldList Def(AstNode* defNode, int isForStruct){
    if(!defNode){
        return NULL;
    }
    AstNode* specifierNode = defNode->leftChild;
    Type type = Specifier(specifierNode);
    return DecList(defNode->leftChild->rightSibling, type, isForStruct);
}

FieldList DecList(AstNode* decListNode, Type type, int isForStruct){
    if(!decListNode){
        return NULL;
    }
    FieldList decField = Dec(decListNode->leftChild, type, isForStruct);
    if(!decField){
        return NULL;
    }
    if(decListNode->leftChild->rightSibling){
        // Dec COMMA DecList
        FieldList decListField  = DecList(decListNode->leftChild->rightSibling->rightSibling, type, isForStruct);
        FieldList decFieldCurrentPtr = decField;
        while (decFieldCurrentPtr->tail)
        {
            decFieldCurrentPtr = decFieldCurrentPtr->tail;
        }
        decFieldCurrentPtr->tail = decListField;
    }
    return decField;
}

FieldList Dec(AstNode* decNode, Type type, int isForStruct){
    if(!decNode){
        return NULL;
    }
    FieldList varList = VarDec(decNode->leftChild, type, isForStruct);
    if(decNode->leftChild->rightSibling){ 
        // VarDec ASSIGNOP Exp
        if(isForStruct){
            printErrorInfo(INIT_STRUCT, decNode->leftChild->lineNO);
            return NULL;
        }
        Type typeExp = Exp(decNode->leftChild->rightSibling->rightSibling);
        if(!isTypeMatched(type, typeExp)){
            printErrorInfo(ASSIGNOP_UNMATCHED, decNode->leftChild->rightSibling->lineNO);
            return NULL;
        }
    }
    return varList;
}

Type Exp(AstNode* expNode){
    if(!expNode){
        return NULL;
    }
    AstNode* child = expNode->leftChild;
    if(child->rightSibling){
        char* firstValue = child->value;
        char* secondValue = child->rightSibling->value;
        if(!strcmp(secondValue, TER_ASSIGNOP)){
            // Exp ASSIGNOP Exp
            // TODO rightvalue?
            Type leftType = Exp(child);
            Type rightType = Exp(child->rightSibling->rightSibling);
            if(!leftType || !rightType){
                return NULL;
            }
            if(leftType->prop == RIGHT_VALUE){
                printErrorInfo(RIGHT_VALUE_LEFT, child->lineNO, "");
                return NULL;
            }
            if(!isTypeMatched(leftType, rightType)){
                printErrorInfo(ASSIGNOP_UNMATCHED, child->rightSibling->lineNO, "");
                return NULL;
            }
            return leftType;
        }
        if(!strcmp(secondValue, TER_AND)
            || !strcmp(secondValue, TER_OR)
            || !strcmp(secondValue, TER_PLUS)
            || !strcmp(secondValue, TER_MINUS)
            || !strcmp(secondValue, TER_STAR)
            || !strcmp(secondValue, TER_DIV)){
                // Exp AND Exp
                // Exp OR Exp
                // Exp PLUS Exp
                // Exp MINUS Exp
                // Exp STAR Exp
                // Exp DIV Exp
                Type leftType = Exp(child);
                Type rightType = Exp(child->rightSibling->rightSibling);
                if(!leftType || !rightType){
                    return NULL;
                }
                if(!isTypeMatched(leftType, rightType)){
                    printErrorInfo(OPER_UNMATCHED, child->rightSibling->lineNO, "");
                    return NULL;
                }
                Type type = (Type)malloc(sizeof(struct Type_));
                memcpy(type, leftType, sizeof(struct Type_));
                type->prop = RIGHT_VALUE;
                return type;
        }
        if(!strcmp(secondValue, TER_RELOP)){
            // Exp RELOP Exp
            Type leftType = Exp(child);
            Type rightType = Exp(child->rightSibling->rightSibling);
            if(!leftType || !rightType){
                return NULL;
            }
            if(!isTypeMatched(leftType, rightType)){
                printErrorInfo(OPER_UNMATCHED, child->rightSibling->lineNO, "");
                return NULL;
            }
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            type->u.basic = INT_TYPE;
            type->prop = RIGHT_VALUE;
            return type;
        }
        if(!strcmp(firstValue, TER_LP)){
            // LP Exp RP
            Type type = Exp(child->rightSibling);
            if(!type){
                return NULL;
            }
            // 一定要重新拷贝，否则改的就是Exp的Type了
            Type res = (Type)malloc(sizeof(struct Type_));
            memcpy(res, type, sizeof(struct Type_));
            res->prop = RIGHT_VALUE;
            return res;
        }
        if(!strcmp(firstValue, TER_MINUS)){
            // MINUS Exp
            Type type = Exp(child->rightSibling);
            if(!type){
                return NULL;
            }
            if(type->kind != BASIC){
                printErrorInfo(OPER_UNMATCHED, child->lineNO, "");
                return NULL;
            }
            // 一定要重新拷贝，否则改的就是Exp的Type了
            Type res = (Type)malloc(sizeof(struct Type_));
            memcpy(res, type, sizeof(struct Type_));
            res->prop = RIGHT_VALUE;
            return res;
        }
        if(!strcmp(firstValue, TER_NOT)){
            // NOT Exp
            Type type = Exp(child->rightSibling);
            if(!type){
                return NULL;
            }
            // 注意这里一定要重新拷贝一份，否则改的就是Exp的Type了
            Type res = (Type)malloc(sizeof(struct Type_));
            memcpy(res, type, sizeof(struct Type_));
            res->prop = RIGHT_VALUE;
            return res;
        }
        if(!strcmp(firstValue, TER_ID)){
            // ID LP Args RP
            // ID LP RP
            char* funcName = child->idValue;
            FieldList paramList = getFuncParamList(funcName);
            if(!isFuncExist(funcName)){
                if(isVarExist(funcName) || isStructExist(funcName)){
                    printErrorInfo(WRONG_FUNC, child->lineNO, funcName);
                    return NULL;
                } else {
                    printErrorInfo(FUNC_NOT_DEFINED, child->lineNO, funcName);
                    return NULL;
                }
            }
            FieldList realParamList = NULL;
            if(!strcmp(child->rightSibling->rightSibling->value, ARGS)){
                realParamList = Args(child->rightSibling->rightSibling);
            }
            if(!checkFuncParam(paramList, realParamList)){
                printErrorInfo(FUNC_PARAM_UNMATCHED, child->lineNO, funcName, funcName);              //=========================
                return NULL;
            }
            Type returnType = getFuncReturnType(funcName);
            Type res = (Type)malloc(sizeof(struct Type_));
            memcpy(res, returnType, sizeof(struct Type_));
            res->prop = RIGHT_VALUE;
            return res;
        }
        if(!strcmp(secondValue, TER_LB)){
            // Exp LB Exp RB
            Type typeVal = Exp(child);
            Type typeIdx = Exp(child->rightSibling->rightSibling);
            if(!typeVal || !typeIdx){
                return NULL;
            }
            if(typeVal->kind != ARRAY){
                AstNode* currentNode = child->leftChild;
                AstNode* fatherNode = child;
                while (currentNode && strcmp(currentNode->value, TER_ID))
                {
                    // 找到ID节点
                    currentNode = currentNode->leftChild;
                    fatherNode = fatherNode->leftChild;
                }
                if(currentNode && !strcmp(currentNode->value, TER_ID)){
                    printErrorInfo(WRONG_ARRAY, child->lineNO, currentNode->idValue);
                } else if(fatherNode->kind == CONST_TYPE){
                    // 源于syntax.y中自底向上传的kind
                    if(!strcmp(fatherNode->value, TER_INT)){
                        fprintf(stderr, "Error type %d at Line %d: \"%ld\" is not an array.\n", WRONG_ARRAY, child->lineNO, fatherNode->intValue);
                    } else if(!strcmp(fatherNode->value, TER_FLOAT)){
                        fprintf(stderr, "Error type %d at Line %d: \"%f\" is not an array.\n", WRONG_ARRAY, child->lineNO, fatherNode->floatValue);
                        // printErrorInfo(WRONG_ARRAY, child->lineNO, fatherNode->idValue);
                    }
                    
                }
                return NULL;
            }
            if(typeIdx->kind != BASIC || typeIdx->u.basic != INT_TYPE){
                double token = child->rightSibling->rightSibling->leftChild->floatValue;
                printErrorInfo(ARRAY_IDX_NOT_INT, child->lineNO, token);
                // printErrorInfo(ARRAY_IDX_NOT_INT, child->lineNO, child->rightSibling->rightSibling->leftChild->idValue);
                return NULL;
            }
            Type type = (Type)malloc(sizeof(struct Type_));
			memcpy(type, typeVal->u.array.elem, sizeof(struct Type_));
			type->prop = LEFT_VALUE;

			return type;
        }
        if(!strcmp(secondValue, TER_DOT)){
            // Exp DOT ID
            Type typeStruct = Exp(child);
            if(!typeStruct){
                return NULL;
            }
            if(typeStruct->kind != STRUCTURE){
                printErrorInfo(WRONG_STRUCT, child->lineNO);
            } else {
                char* varName = child->rightSibling->rightSibling->idValue;
                FieldList structVarPtr = typeStruct->u.structure;
                Type varTrueType = NULL;
                while (structVarPtr)
                {
                    if(!strcmp(structVarPtr->name, varName)){
                        varTrueType = structVarPtr->type;
                    }
                    structVarPtr = structVarPtr->tail;
                }
                if(varTrueType){
                    return varTrueType;
                } else {
                    printErrorInfo(STRUCT_FIELD_NOT_DEFINED, child->rightSibling->rightSibling->lineNO, varName);
                    return NULL;
                }
            }
        }
    } else {
        if(!strcmp(child->value, TER_ID)){
            if(!isVarExist(child->idValue) && !isStructExist(child->idValue)){
                // printSignalList();
                // fprintf(stderr, "\n601");
                printErrorInfo(VAR_NOT_DEFINED, child->lineNO, child->idValue);
                return NULL;
            }else {
                Type type = getVarType(child->idValue);
                return type;
            }
        }
        if(!strcmp(child->value, TER_INT)){
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            type->u.basic = INT_TYPE;
            type->prop = RIGHT_VALUE;
            return type;
        }
        if(!strcmp(child->value, TER_FLOAT)){
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = BASIC;
            type->u.basic = FLOAT_TYPE;
            type->prop = RIGHT_VALUE;
            return type;
        }
    }
}

FieldList Args(AstNode* argsNode){
    if(!argsNode){
        return NULL;
    }
    AstNode* expNode = argsNode->leftChild;
    Type type = Exp(expNode);
    if(!type){
        return NULL;
    }
    FieldList fieldList = (FieldList)malloc(sizeof(struct FieldList_));
    // 注意这里name是NULL的，没有设！！！
    fieldList->type = type;
    if(expNode->rightSibling){
        fieldList->tail = Args(expNode->rightSibling->rightSibling);
    }
    return fieldList;
}

void printErrorInfo(int errorType, int lineNO, ...){
    va_list args;
    va_start(args, lineNO);
    if(errorType == INIT_STRUCT){
        fprintf(stderr,"Error type 15 at Line %d: Unsupoorted initialized.\n", lineNO);
    } else {
        char* s = NULL;
        char* s1 = NULL;
        char* s2 = NULL;
        double floatNum;
        fprintf(stderr, "Error type %d at Line %d: ", errorType, lineNO);
        switch (errorType)
        {
            case VAR_NOT_DEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Undefined variable \"%s\"\n", s);
                break;
            case FUNC_NOT_DEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Undefined function \"%s\"\n", s);
                break;
            case VAR_REDEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Redefined variable \"%s\"\n", s);
                break;
            case FUNC_REDEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Redefined function \"%s\"\n", s);
                break;
            case ASSIGNOP_UNMATCHED:
                fprintf(stderr,"Type mismatched for assignment.\n");
                break;
            case RIGHT_VALUE_LEFT:
                fprintf(stderr,"The left-hand side of an assignment must be a variable.\n");
                break;
            case OPER_UNMATCHED:
                fprintf(stderr,"Type mismatched for operands.\n");
                break;
            case RETURN_UNMATCHED:
                fprintf(stderr,"Type mismatched for return.\n");
                break;
            case FUNC_PARAM_UNMATCHED:
                s1 = va_arg(args, char*);
                s2 = va_arg(args, char*);
                fprintf(stderr,"Function \"%s\" is not applicable for arguments \"%s\".\n", s1, s2);
                break;
            case WRONG_ARRAY:
                s = va_arg(args, char*);
                fprintf(stderr,"\"%s\" is not an array.\n", s);
                break;
            case WRONG_FUNC:
                s = va_arg(args, char*);
                fprintf(stderr,"\"%s\" is not a function.\n", s);
                break;
            case ARRAY_IDX_NOT_INT:
                floatNum = va_arg(args, double);
                fprintf(stderr,"\"%f\" is not an integer.\n", floatNum);
                // s = va_arg(args, char*);
                // fprintf(stderr,"\"%s\" is not an integer.\n", s);
                break;
            case WRONG_STRUCT:
                fprintf(stderr,"Illegal use of \".\".\n");
                break;
            case STRUCT_FIELD_NOT_DEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Non-existent field \"%s\".\n", s);
                break;
            case ERROR_STRUCT_FIELD:
                s = va_arg(args, char*);
                fprintf(stderr,"Redefined field \"%s\".\n", s);
                break;
            case STURCT_REDEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Duplicated name \"%s\".\n", s);
                break;
            case STRUCT_NOT_DEFINED:
                s = va_arg(args, char*);
                fprintf(stderr,"Undefined structure \"%s\".\n", s);
                break;
            default:
                break;
        }
    }
}