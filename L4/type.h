#ifndef TYPE_H
#define TYPE_H

#define INT_TYPE 0
#define FLOAT_TYPE 1

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Var_* Var;
typedef struct Structure_* Structure;
typedef struct Function_* Function;
// typedef struct Array_* Array;

extern char* INT_TYPE_NAME;
extern char* FLOAT_TYPE_NAME;

// 为了结构体内部定义的判断
extern FieldList currentStructHead;
extern FieldList currentStructTail;


struct Type_{
    enum {BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        FieldList structure;
    } u; 
    enum { LEFT_VALUE, RIGHT_VALUE } prop;
};

struct FieldList_
{
    char* name;      // 域的名字
    Type type;       // 域的类型
    FieldList tail;  // 下一个域
};

// 变量（包括普通变量和数组）
struct Var_{
    char* name;
    Type type;
    struct Var_* next;
};

struct Structure_{
	char *name;
	FieldList domain;
    struct Structure_* next;
};

struct Function_{
	char* name;
	int line;
	Type type;
	FieldList parameter;
    struct Function_* next;
};

void init();

void initCurrentStruct();
FieldList backupCurrentStruct(FieldList backup);
void recoverCurrentStruct(FieldList backup);
int isVarInCurrentStruct(char* varName);
void addVarInCurrentStruct(char* name, Type type);
void printCurrentStructField();

// 类型比较
int isTypeMatched(Type type1, Type type2);
int isStructTypeMatched(FieldList domain1, FieldList domain2);
int checkFuncParam(FieldList paramList1, FieldList paramList2);

// 变量表
void addVar(char* name, Type type);
int isVarExist(char* name);
Type getVarType(char* name);

// 函数表
void addFunc(char* name, int line, Type type, FieldList param);
int isFuncExist(char* name);
FieldList getFuncParamList(char* name);
Type getFuncReturnType(char* name);

// 结构体表
void addStruct(char* name, FieldList domain);
int isStructExist(char* name);
int isVarExistInStruct(char* structName, char* varName);
Type getStructType(char* name);
Type getVarTypeInStruct(char* structName, char* varName);

void printSignalList();

#endif