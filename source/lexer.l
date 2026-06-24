%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../include/tokens.h"
#include "../include/syntax_tree.h"
#include "../include/memory.h"

extern void yyerror(const char*);
extern int yyleng;
extern bool analysis_error;

char* copy_substring(const char*, int, int, bool);

#define YY_USER_ACTION                        \
    yylloc.first_line = yylloc.last_line;     \
    yylloc.first_column = yylloc.last_column; \
    for (int i = 0; i < yyleng; i++) {        \
        if (yytext[i] == '\n') {              \
            yylloc.last_line++;               \
            yylloc.last_column = 1;           \
        }                                     \
        else {                                \
            yylloc.last_column++;             \
        }                                     \
    }                                         \

%}

%option yylineno
%option noyywrap
%option debug

DIGIT [[:digit:]]
ID [[:alpha:]_][[:alnum:]_]*

%x comentario
%x cadeiacaracteres

%%

"/*" {
    BEGIN(comentario);
}

<comentario>{
    [^\*]* ;
    "*"+[^/] ;
    <<EOF>> {
        yyerror("Comentário não termina");
    }
    "*"+"/" BEGIN(INITIAL);
}

"\"" {
    BEGIN(cadeiacaracteres);
}

<cadeiacaracteres>{
    ([^\\\"\n]|\\.)*\" {
        yylval.lexeme = copy_substring(yytext, 0, strlen(yytext) - 2, false);
        BEGIN(INITIAL);
        return CADEIACARACTERES;
    }
    <<EOF>> {
        yyerror("Cadeia de caracteres nao termina");
    }
    [\n] {
        yyerror("Cadeia de caracteres ocupa mais de uma linha");
    }
    . {
        yyerror("Caractere inválido");
    }
}

'([^\\\'\n]|\\.)' {
    yylval.lexeme = copy_substring(yytext, 1, strlen(yytext) - 2, true);
    return CARCONST;
}

'' {
    yyerror("Caractere deve conter pelo menos um símbolo");
}

'.*' {
    yyerror("Caractere com mais de um símbolo");
}

"+"         {return '+';}
"-"         {return '-';}
"*"         {return '*';}
"/"         {return '/';}
"{"         {return '{';}
"}"         {return '}';}
"("         {return '(';}
")"         {return ')';}
"["         {return '[';}
"]"         {return ']';}
","         {return ',';}
";"         {return ';';}
":"         {return ':';}
"!"         {return '!';}
"="         {return '=';}
"<"         {return '<';}
">"         {return '>';}
"&"         {return E;}
"||"        {return OU;}
"=="        {return IGUAL;}
"!="        {return DIFERENTE;}
"<="        {return MENORIGUAL;}
">="        {return MAIORIGUAL;}
"global"    {return GLOBAL;}
"funcao"    {return FUNCAO;}
"principal" {return PRINCIPAL;}
"retorne"   {return RETORNE;}
"int"       {return INT;}
"car"       {return CAR;}
"leia"      {return LEIA;}
"escreva"   {return ESCREVA;}
"novalinha" {return NOVALINHA;}
"se"        {return SE;}
"entao"     {return ENTAO;}
"senao"     {return SENAO;}
"fimse"     {return FIMSE;}
"enquanto"  {return ENQUANTO;}

{DIGIT}+ {
    yylval.lexeme = copy_substring(yytext, 0, strlen(yytext) - 1, false);
    return INTCONST;
}

{ID} {
    yylval.lexeme = copy_substring(yytext, 0, strlen(yytext) - 1, false);
    return IDENTIFICADOR;
}

[ \n\t]+ ;

<<EOF>> {return EOF;}

. {
    yyerror("Caractere inválido");
}

%%

/**
 * @brief Copy a substring of a string
 * 
 * @param string String to be copied
 * @param left Initial position in string to be copied (begin of substring)
 * @param right Final position in string to be copied (end of substring)
 * @param escape If true, process escape characters. Else, does not process.
 * @return char* The substring of string copied in new allocated memory block
 */
char* copy_substring(const char* string, int left, int right, bool escape) {
    int length = strlen(string);
    char* copy = (char*)allocate_memory(length + 1);

    for (int i = 0; i <= length; i++) {
        copy[i] = '\0';
    }

    if (escape) {
        for (int i = 0, j = left; i < length && j <= right; i++, j++) {
            if (string[j] == '\\' && j + 1 <= right) {
                switch (string[j + 1]) {
                    case 'a':
                        copy[i] = '\a';
                        break;
                    case 'b':
                        copy[i] = '\b';
                        break;
                    case 'f':
                        copy[i] = '\f';
                        break;
                    case 'n':
                        copy[i] = '\n';
                        break;
                    case 'r':
                        copy[i] = '\r';
                        break;
                    case 't':
                        copy[i] = '\t';
                        break;
                    case 'v':
                        copy[i] = '\v';
                        break;
                    default:
                        copy[i] = string[j + 1];
                        continue;
                }

                j++;
            }
            else {
                copy[i] = string[j];
            }
        }
    }
    else {
        for (int i = 0, j = left; i < length && j <= right; i++, j++) {
            if (string[j] == '\\' && j + 1 <= right) {
                char c = string[j + 1];

                if (c == 'a' || c == 'b' || c == 'f' ||
                    c == 'n' || c == 'r' || c == 't' ||
                    c == 'v' || c == '\'' || c == '\"') {
                    copy[i] = string[j];
                }
                else {
                    copy[i] = c;
                    j++;
                }
            }
            else {
                copy[i] = string[j];
            }
        }
    }

    return copy;
}