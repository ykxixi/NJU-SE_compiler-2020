#ifndef TOOL_H
#define TOOL_H

extern int yylex(void);
extern int yyparse(void);
extern void yyrestart(FILE*);
extern int yylineno;
extern int yyerror(char*);

#endif