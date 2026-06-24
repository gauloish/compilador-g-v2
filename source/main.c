#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/memory.h"

#define DEBUG_OFF 0
#define DEBUG_ON  1

extern FILE* yyin;
extern int yydebug;
extern int yy_flex_debug;
extern int yyparse();
extern void yyerror(char const *);
extern int analysis_error;

char* output_file = NULL;

int main(int argc, char** argv){
    if (argc == 2) {
        output_file = "assembly.s";
    }
    else if (argc == 3) {
        output_file = argv[2];
    }
    else {
        analysis_error = false;
        yyerror("Uso correto \"./g-v2 <entrada> <saida>\"!!!");
    }

    yyin = fopen(argv[1], "r");

    if (!yyin) {
        yyerror("Arquivo não pode ser aberto!!!");
    }

    yydebug = DEBUG_OFF;
    yy_flex_debug = DEBUG_OFF;

    yyparse();
    fclose(yyin);
}