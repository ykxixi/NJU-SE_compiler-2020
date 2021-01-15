#include"type.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

char* INT_TYPE_NAME = "int";
char* FLOAT_TYPE_NAME = "float";

Var varListHead;            // 虚节点
Var varListTail;            // 指向最后一个节点
Function funcListHead;
Function funcListTail;
Structure structListHead;
Structure structListTail;
FieldList currentStructHead;
FieldList currentStructTail;

void copyStruct(FieldList srcField, FieldList destField);

void init(){
    varListHead = (Var)malloc(sizeof(struct Var_));
    varListTail = varListHead;

    funcListHead = (Function)malloc(sizeof(struct Function_));
    funcListTail = funcListHead;

    structListHead = (Structure)malloc(sizeof(struct Structure_));
    structListTail = structListHead;

    initCurrentStruct();
}

void initCurrentStruct(){
    currentStructHead = (FieldList)malloc(sizeof(struct FieldList_));
    currentStructTail = currentStructHead;
    currentStructHead->tail = NULL;

    // fprintf(stderr, "init--   ");
    // printCurrentStructField();
}

FieldList backupCurrentStruct(FieldList backup){
    if(currentStructHead->tail){
        // fprintf(stderr, "backup\n");
        copyStruct(currentStructHead->tail, backup);
        return backup;
    } else {
        return NULL;
    }
}

void recoverCurrentStruct(FieldList backup){
    initCurrentStruct();
    if(backup){
        // fprintf(stderr, "recover\n");
        currentStructHead->tail = backup;

        FieldList currentPtr = currentStructHead->tail;
        while (currentPtr->tail)
        {
            currentPtr = currentPtr->tail;
        }
        currentStructTail = currentPtr;
    }
}

void copyStruct(FieldList srcField, FieldList destField){
    if(!srcField){
        return;
    }
    // destField = (FieldList)malloc(sizeof(struct FieldList_));
    destField->name = (char*)malloc(sizeof(strlen(srcField->name) + 1));
    if(srcField->name){
        strcpy(destField->name, srcField->name);
    }
    destField->type = (Type)malloc(sizeof(struct Type_));
    memcpy(destField->type, srcField->type, sizeof(struct Type_));
    FieldList srcFieldPtr = srcField->tail;
    FieldList destFieldPtr = destField;
    while (srcFieldPtr)
    {
        FieldList tmp = (FieldList)malloc(sizeof(struct FieldList_));
        tmp->name = (char*)malloc(sizeof(strlen(srcFieldPtr->name) + 1));
        if(srcFieldPtr->name){
            strcpy(tmp->name, srcFieldPtr->name);
        }
        tmp->type = (Type)malloc(sizeof(struct Type_));
        memcpy(tmp->type, srcFieldPtr->type, sizeof(struct Type_));
        destFieldPtr->tail = tmp;
        destFieldPtr = destFieldPtr->tail;
        srcFieldPtr = srcFieldPtr->tail;
    }
}

int isVarInCurrentStruct(char* varName){
    FieldList currentPtr = currentStructHead->tail;
    while (currentPtr)
    {
        if(currentPtr->name && !strcmp(currentPtr->name, varName)){
            return 1;
        }
        currentPtr = currentPtr->tail;
    }
    return 0;
}

void addVarInCurrentStruct(char* name, Type type){
    // fprintf(stderr, "add name: %s\n", name);
    FieldList newVar = (FieldList)malloc(sizeof(struct FieldList_));
    newVar->name = name;
    newVar->type = type;
    currentStructTail->tail = newVar;
    currentStructTail = newVar;
}

void printCurrentStructField(){
    FieldList currentFieldPtr = currentStructHead->tail;
    while (currentFieldPtr)
    {
        fprintf(stderr, "local: %s\n", currentFieldPtr->name);
        currentFieldPtr = currentFieldPtr->tail;
    }
}

/**
 * 类型比较
 */
int isTypeMatched(Type type1, Type type2){
    if((!type1) && (!type2)){
        // 均为NULL，认为相等
        return 1;
    } 
    if((!type1) || (!type2)){
        // 有且仅有一个NULL
        return 0;
    }
    // 至此均不为NULL

    if(type1->kind != type2->kind){
        return 0;
    }
    // 至此kind均相同

    switch (type1->kind)
    {
        case BASIC:
            if(type1->u.basic != type2->u.basic){
                return 0;
            } else{
                return 1;
            }
        case ARRAY:
            return isTypeMatched(type1->u.array.elem, type2->u.array.elem);
        case STRUCTURE:
            return isStructTypeMatched(type1->u.structure, type2->u.structure);
        default:
            break;
    }
}

// 判断结构体结构等价
int isStructTypeMatched(FieldList domain1, FieldList domain2){
    FieldList domain1Ptr = domain1;
    FieldList domain2Ptr = domain2;
    while (domain1Ptr && domain2Ptr)
    {
        if(!isTypeMatched(domain1Ptr->type, domain2Ptr->type)){
            return 0;
        }
        domain1Ptr = domain1Ptr->tail;
        domain2Ptr = domain2Ptr->tail;
    }
    return (!domain1Ptr) && (!domain2Ptr);
}

int checkFuncParam(FieldList paramList1, FieldList paramList2){
    FieldList paramList1Ptr = paramList1;
    FieldList paramList2Ptr = paramList2;
    while (paramList1Ptr && paramList2Ptr)
    {
        if(!isTypeMatched(paramList1Ptr->type, paramList2Ptr->type)){
            return 0;
        }
        paramList1Ptr = paramList1Ptr->tail;
        paramList2Ptr = paramList2Ptr->tail;
    }
    return (!paramList1Ptr) && (!paramList2Ptr);
}

/**
 * 变量表
 */
void addVar(char* name, Type type){
    Var newNode = (Var)malloc(sizeof(struct Var_));
    newNode->name = name;
    newNode->type = type;
    varListTail->next = newNode;
    varListTail = newNode;
}

int isVarExist(char* name){
    Var currentPtr = varListHead->next;
    while (currentPtr)
    {
        if(!strcmp(currentPtr->name, name)){
            return 1;
        }
        currentPtr = currentPtr->next;
    }
    return 0;
}

Type getVarType(char* name){
    Var currentPtr = varListHead->next;
    while (currentPtr)
    {
        if(!strcmp(currentPtr->name, name)){
            return currentPtr->type;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

// Type getVarTypeInStructVar(char* structVarName, char* varName){
//     fprintf(stderr, "to find: %s %s\n", structVarName, varName);
//     Var currentPtr = varListHead->next;
//     while (currentPtr)
//     {
//         fprintf(stderr, "outer: %s\n", currentPtr->name);
//         if(!strcmp(currentPtr->name, structVarName)){
//             FieldList varListPtr = currentPtr->type->u.structure;
//             fprintf(stderr, "loop: %d\n", currentPtr->type->kind);
//             while (varListPtr)
//             {
//                 if(!strcmp(varListPtr->name, varName)){
//                     return varListPtr->type;
//                 }
//                 varListPtr = varListPtr->tail;
//             }
//             return NULL;
//         }
//         currentPtr = currentPtr->next;
//     }
//     return NULL;
// }

void addFunc(char* name, int line, Type type, FieldList param){
    Function newNode = (Function)malloc(sizeof(struct Function_));
    newNode->name = name;
    newNode->line = line;
    newNode->type = type;
    newNode->parameter = param;
    funcListTail->next = newNode;
    funcListTail = newNode;
}

int isFuncExist(char* name){
    Function currentPtr = funcListHead->next;
    while (currentPtr)
    {
        if(!strcmp(currentPtr->name, name)){
            return 1;
        }
        currentPtr = currentPtr->next;
    }
    return 0;
}

FieldList getFuncParamList(char* name){
    Function currentPtr = funcListHead->next;
    while (currentPtr)
    {
        if(!strcmp(currentPtr->name, name)){
            return currentPtr->parameter;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

Type getFuncReturnType(char* name){
    Function currentPtr = funcListHead->next;
    while (currentPtr)
    {
        if(!strcmp(currentPtr->name, name)){
            return currentPtr->type;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

/**
 * 结构体表
 */
void addStruct(char* name, FieldList domain){
    Structure newNode = (Structure)malloc(sizeof(struct Structure_));
    newNode->name = name;
    newNode->domain = domain;
    structListTail->next = newNode;
    structListTail = newNode;
}

int isStructExist(char* name){
    Structure currentPtr = structListHead->next;
    if(!name || !strcmp(name, "")){
        // 这里将无名结构体都认为是存在了，但无名结构体除了定义时，后面好像没有访问过？
        return 1;
    }
    while (currentPtr)
    {
        // 注意!NULL名的不判断就用strcmp会报错
        if(currentPtr->name && !strcmp(currentPtr->name, name)){
            return 1;
        }
        currentPtr = currentPtr->next;
    }
    return 0;
}

int isVarExistInStruct(char* structName, char* varName){
    Structure currentPtr = structListHead->next;
    while (currentPtr)
    {
        if(currentPtr->name && !strcmp(currentPtr->name, structName)){
            FieldList fieldList = currentPtr->domain;
            while (fieldList)
            {
                if(!strcmp(fieldList->name, varName)){
                    return 1;
                }
                fieldList = fieldList->tail;
            }
            break;
        }
        currentPtr = currentPtr->next;
    }
    return 0;
}

Type getStructType(char* name){
    Structure currentPtr = structListHead->next;
    while (currentPtr)
    {
        if(currentPtr->name && !strcmp(currentPtr->name, name)){
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = STRUCTURE;
            type->u.structure = currentPtr->domain;
            return type;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

Type getVarTypeInStruct(char* structName, char* varName){
    // fprintf(stderr, "to find: %s %s\n", structName, varName);
    Structure currentPtr = structListHead->next;
    while (currentPtr)
    {
        if(currentPtr->name && !strcmp(currentPtr->name, structName)){
            FieldList currentFieldPtr = currentPtr->domain;
            while (currentFieldPtr)
            {
                if(!strcmp(currentFieldPtr->name, varName)){
                    return currentFieldPtr->type;
                }
                currentFieldPtr = currentFieldPtr->tail;
            }
            return NULL;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

void printSignalList(){
    Var currentVarPtr = varListHead->next;
    while (currentVarPtr)
    {
        fprintf(stderr, "var: %s\n", currentVarPtr->name);
        currentVarPtr = currentVarPtr->next;
    }

    Function currentFuncPtr = funcListHead->next;
    while (currentFuncPtr)
    {
        fprintf(stderr, "func: %s\n", currentFuncPtr->name);
        currentFuncPtr = currentFuncPtr->next;
    }
    
    Structure currentStructPtr = structListHead->next;
    while (currentStructPtr)
    {
        fprintf(stderr, "struct: %s\n", currentStructPtr->name);
        currentStructPtr = currentStructPtr->next;
    }
}

