#ifndef ASTNODE_H
#define ASTNODE_H

//1为变量，2为函数，3为常数,4为数组，5为结构体
typedef enum TYPE_KIND_{ BASIC_TYPE=1, FUNC_TYPE, CONST_TYPE, ARRAY_TYPE, STRUCT_TYPE } TYPE_KIND;

/**
 * 结构体
 */
typedef struct Node
{
    int lineNO;
    char* value;
    struct Node* leftChild;
    struct Node* rightSibling;
    TYPE_KIND kind;
    union {
        char* idValue;
        char* typeValue;
        long intValue;
        double floatValue;
    };
} AstNode;

extern AstNode* setAstNode(char* v, int vNum, ...);
extern void traversal(AstNode* syntax_root, int depth);

#endif