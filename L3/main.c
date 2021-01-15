#include<stdio.h>
#include"AstNode.h"
#include"tool.h"
#include"semantic.h"
#include"type.h"

int yyerror(char* s);

extern int syntax_err;
AstNode* syntax_root;

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


    if(!syntax_err){
        if(syntax_root){
            init();
            // traversal(syntax_root, 0);
            ExtDefList(syntax_root->leftChild);
            // printSignalList();
        }
    }

    return 0;
}


int yyerror(char* s) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, s);
    return 0;
}


