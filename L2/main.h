#pragma once
#include<stdarg.h>
#include<stdio.h>

extern int yylineno;


char* Id = "ID";
char* Type = "TYPE";
char* Int = "INT";
char* Float = "FLOAT";
char* Epsilon = "EPSILON";

struct AstNode
{
    int lineNO;
    char* value;
    struct AstNode* leftChild;
    struct AstNode* rightSibling;
    union {
        char* idValue;
        char* typeValue;
        long intValue;
        double floatValue;
    };
};

struct AstNode* setAstNode(char* v, int vNum, ...) {
    struct AstNode* astnode = (struct AstNode*)malloc(sizeof(struct AstNode));
    astnode->value = v;
    //定义一个可变参数列表
    va_list args;
    //初始化args指向强制参数arg的下一个参数
    va_start(args, vNum);

    if (vNum == 0) {
        // 是终结符
        astnode->lineNO = va_arg(args, int);
        if (!strcmp(v, Id)) {
            char* va_id = va_arg(args, char*);
            // 初始化，否则段错误
            astnode->idValue = (char*)malloc(sizeof(char) * 32);
            // 拷贝！！！否则还是指向这个位置的指针
            strcpy(astnode->idValue, va_id);
        }
        else if (!strcmp(v, Type)) {
            char* va_type = va_arg(args, char*);
            astnode->typeValue = (char*)malloc(sizeof(char) * 32);
            strcpy(astnode->typeValue, va_type);
        }
        else if (!strcmp(v, Int)) {
            astnode->intValue = va_arg(args, long);
        }
        else if (!strcmp(v, Float)) {
            // va_arg如果是float会发生类型转换到double，导致出错
            astnode->floatValue = va_arg(args, double);
        }
    }
    else
    {
        astnode->leftChild = va_arg(args, struct AstNode*);
        astnode->lineNO = astnode->leftChild->lineNO;
        struct AstNode* currentChild = astnode->leftChild;
        for (int i = 1; i < vNum; i++) {
            currentChild->rightSibling = va_arg(args, struct AstNode*);
            currentChild = currentChild->rightSibling;
        }
    }
    return astnode;
}

void traversal(struct AstNode* root, int depth) {
    if (root->lineNO != -1) {
        // 不是\epsilon，也没产生\epsilon
        
        // 打印自己
    	for (int i = 0; i < depth; i++) {
            fprintf(stderr, "  ");
    	}
    	fprintf(stderr, "%s", root->value);
    	
    	if (!strcmp(root->value, Id)) {
            fprintf(stderr, "%s%s", ": ", root->idValue);
    	}
    	else if (!strcmp(root->value, Type)) {
	    fprintf(stderr, "%s%s", ": ", root->typeValue);
	}
        else if (!strcmp(root->value, Int)) {
            // 这里不用ld有可能溢出
    	    fprintf(stderr, "%s%ld", ": ", root->intValue);
	}
	else if (!strcmp(root->value, Float)) {
	    fprintf(stderr, "%s%f", ": ", root->floatValue);
	}
	else if(root->leftChild) {
	    // 非终结符才需要打印行号
	    fprintf(stderr, "%s%d%s", " (", root->lineNO, ")");
	}
	fprintf(stderr, "\n");
    }
    
    // 孩子
    if (root->leftChild) {
        traversal(root->leftChild, depth + 1);
    }

    // 兄弟
    if (root->rightSibling) {
        traversal(root->rightSibling, depth);
    }
    
    // 到这里意味着不会再被用到，释放申请的堆空间
    free(root);
}

int yyerror(char* s) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, s);
    return 0;
}
