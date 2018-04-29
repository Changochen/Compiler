#include <iostream>
#include <cstdio>
#include "node.h"
#include <cstdlib>
using namespace std;

extern int yyparse();
extern int yylex();
extern int yyrestart(FILE* f);
extern NBlock* programBlock;
//extern std::vector<NExtDef*>* programBlock;
extern int yylineno;

int main(int argc, char** argv)
{
//    printf("%d\n",yytokentype::TNOT);
    if (argc < 2) {
        yylex();
        return 0;
    }
    int i = 0;
    for (i = 1; i < argc; i++) {
        FILE* f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        yylineno = 1;
        //yydebug = 1;
        yyparse();
        fclose(f);
        programBlock->print(0);
    }
    return 0;
}
