#include<stdio.h>
using namespace std;

extern "C" int yyrestart(FILE*);
extern "C" int yyparse();

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
    
    yyrestart(fp);
    yyparse();

    return 0;
}

