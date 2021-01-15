#include<stdarg.h>
#include<stdio.h>
#include<vector>
#include<iostream>
#include<cstring>
#include "AstNode.h"
using namespace std;

extern "C" int yyparse();

const char* ID = "ID";
const char* TYPE = "TYPE";
const char* INT = "INT";
const char* FLOAT = "FLOAT";
const char* EPSILON = "EPSILON";

//int main() {
//    /*fprintf(stderr, "%s", typeid("s").name());*/
//    AstNode* node1 = setAstNode(const_cast<char*>(TYPE), 0, 2, "float");
//    auto* node2 = setAstNode(const_cast<char*>(EPSILON), 0, -1);
//    auto* node3 = setAstNode(const_cast<char*>(ID), 0, 3, "sub");
//    auto* node4 = setAstNode(const_cast<char*>(INT), 0, 4, 0x1a);
//    auto* node5 = setAstNode(const_cast<char*>(FLOAT), 0, 4, 5.5e-3);
//    auto* node6 = setAstNode(const_cast<char*>("Ext"), 1, node1);
//    auto* node7 = setAstNode(const_cast<char*>("Ext"), 3, node6, node2, node3);
//    auto* node8 = setAstNode(const_cast<char*>("Program"), 3, node7, node4, node5);
//    traversal(node8, 0);
//}

int main(int argc, char** argv) {
    if (argc <= 1)
        return 1;

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror(argv[1]);
        return 1;
    }

    yyparse();

    return 0;
}

int yyerror(char* s) {
    fprintf(stderr, "%s%d%s%s", "Error type B at Line ", yylineno, ": ", s);
    return 0;
}

struct AstNode* setAstNode(char* v, int vNum, ...) {
    struct AstNode* astnode = new AstNode;
    astnode->value = v;
    //����һ���ɱ�����б�
    va_list args;
    //��ʼ��argsָ��ǿ�Ʋ���arg����һ������
    va_start(args, vNum);

    if (vNum == 0) {
        // ���ս��
        astnode->lineNO = va_arg(args, int);
        if (!strcmp(v, ID)) {
            astnode->idValue = va_arg(args, char*);
        }
        else if (!strcmp(v, TYPE)) {
            astnode->typeValue = va_arg(args, char*);
        }
        else if (!strcmp(v, INT)) {
            astnode->intValue = va_arg(args, int);
        }
        else if (!strcmp(v, FLOAT)) {
            // va_arg�����float�ᷢ������ת����double�����³���
            astnode->floatValue = va_arg(args, double);
        }
    }
    else
    {
        astnode->leftChild = va_arg(args, AstNode*);
        astnode->lineNO = astnode->leftChild->lineNO;
        AstNode* currentChild = astnode->leftChild;
        for (int i = 1; i < vNum; i++) {
            currentChild->rightSibling = va_arg(args, AstNode*);
            currentChild = currentChild->rightSibling;
        }
    }
    return astnode;
}

void traversal(struct AstNode* root, int depth) {
    if (root->lineNO == -1) {
        // \epsilon�������\epsilon
        return;
    }
    // ��ӡ�Լ�
    for (int i = 0; i < depth; i++) {
        fprintf(stderr, "  ");
    }
    fprintf(stderr, "%s", root->value);
    if (!strcmp(root->value, ID)) {
        fprintf(stderr, "%s%s", ": ", root->idValue);
    }
    else if (!strcmp(root->value, TYPE)) {
        fprintf(stderr, "%s%s", ": ", root->typeValue);
    }
    else if (!strcmp(root->value, INT)) {
        fprintf(stderr, "%s%d", ": ", root->intValue);
    }
    else if (!strcmp(root->value, FLOAT)) {
        fprintf(stderr, "%s%f", ": ", root->floatValue);
    }
    else
    {
        fprintf(stderr, "%s%d%s", " (", root->lineNO, ")");
    }
    fprintf(stderr, "\n");

    // ��ӡ�ֵܽڵ�
    struct AstNode* currentSibling = root->rightSibling;
    while (currentSibling) {
        traversal(currentSibling, depth);
        currentSibling = currentSibling->rightSibling;
    }

    // ��ӡ�ӽڵ�
    if (root->leftChild) {
        traversal(root->leftChild, depth + 1);
    }
}
