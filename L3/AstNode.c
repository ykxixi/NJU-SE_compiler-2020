#include "AstNode.h"
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

AstNode* setAstNode(char* v, int vNum, ...) {
    AstNode* astnode = (AstNode*)malloc(sizeof(AstNode));
    astnode->value = v;
    //定义一个可变参数列表
    va_list args;
    //初始化args指向强制参数arg的下一个参数
    va_start(args, vNum);

    if (vNum == 0) {
        // 是终结符
        astnode->lineNO = va_arg(args, int);
        if (!strcmp(v, "ID")) {
            char* va_id = va_arg(args, char*);
            // 初始化，否则段错误
            astnode->idValue = (char*)malloc(sizeof(char) * 32);
            // 拷贝！！！否则还是指向这个位置的指针
            strcpy(astnode->idValue, va_id);
        }
        else if (!strcmp(v, "TYPE")) {
            char* va_type = va_arg(args, char*);
            astnode->typeValue = (char*)malloc(sizeof(char) * 32);
            strcpy(astnode->typeValue, va_type);
        }
        else if (!strcmp(v, "INT")) {
            astnode->intValue = va_arg(args, long);
        }
        else if (!strcmp(v, "FLOAT")) {
            // // va_arg如果是float会发生类型转换到double，导致出错
            // char* va_float = va_arg(args, char*);
            // // 初始化，否则段错误
            // astnode->idValue = (char*)malloc(sizeof(char) * 32);
            // // 拷贝！！！否则还是指向这个位置的指针
            // strcpy(astnode->idValue, va_float);
            double floatValue = va_arg(args, double);
            astnode->floatValue = floatValue;
        }
    }
    else
    {
        astnode->leftChild = va_arg(args, AstNode*);
        astnode->lineNO = astnode->leftChild->lineNO;
        AstNode* currentChild = astnode->leftChild;
        if(vNum == 1){
            // 只有一个孩子，父节点的语义值等于左孩子的语义值
            astnode->idValue = currentChild->idValue;
            astnode->kind = currentChild->kind;
        }
        else
        {
            for (int i = 1; i < vNum; i++) {
                currentChild->rightSibling = va_arg(args, AstNode*);
                currentChild = currentChild->rightSibling;
            }
        }
        
    }
    return astnode;
}

void traversal(AstNode* syntax_root, int depth) {
    if (syntax_root->lineNO != -1) {
        // 不是\epsilon，也没产生\epsilon
        
        // 打印自己
    	for (int i = 0; i < depth; i++) {
            fprintf(stderr, "  ");
    	}
    	fprintf(stderr, "%s", syntax_root->value);
    	
    	if (!strcmp(syntax_root->value, "ID")) {
            fprintf(stderr, "%s%s", ": ", syntax_root->idValue);
    	}
    	else if (!strcmp(syntax_root->value, "TYPE")) {
	        fprintf(stderr, "%s%s", ": ", syntax_root->typeValue);
	    }
        else if (!strcmp(syntax_root->value, "INT")) {
            // 这里不用ld有可能溢出
    	    fprintf(stderr, "%s%ld", ": ", syntax_root->intValue);
	    }
	    else if (!strcmp(syntax_root->value, "FLOAT")) {
            fprintf(stderr, "%s%f", ": ", syntax_root->floatValue);
        }
        else if(syntax_root->leftChild) {
            // 非终结符才需要打印行号
            fprintf(stderr, "%s%d%s", " (", syntax_root->lineNO, ")");
        }
	    fprintf(stderr, "\n");
    }
    
    // 孩子
    if (syntax_root->leftChild) {
        traversal(syntax_root->leftChild, depth + 1);
    }

    // 兄弟
    if (syntax_root->rightSibling) {
        traversal(syntax_root->rightSibling, depth);
    }
    
    // 到这里意味着不会再被用到，释放申请的堆空间
    // free(syntax_root);
}
