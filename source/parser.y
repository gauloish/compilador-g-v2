%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/memory.h"
#include "../include/syntax_tree.h"
#include "../include/semantic_analysis.h"
#include "../include/code_generation.h"

extern int yylex();
extern char* yytext;
extern int yyleng;
extern int yylineno;

void yyerror(const char*);

TreeNode* tree;
bool analysis_error = true;
%}

%define parse.trace
%define parse.error custom

%locations

%code requires {
    #include "../include/syntax_tree.h"
}

%union {
    char* lexeme;
    TreeNode* node;
}

%start Programa

%token GLOBAL           "'global'"
%token FUNCAO           "'funcao'"
%token PRINCIPAL        "'principal'"
%token INT              "'int'"
%token CAR              "'car'"
%token RETORNE          "'retorne'"
%token LEIA             "'leia'"
%token ESCREVA          "'escreva'"
%token NOVALINHA        "'novalinha'"
%token SE               "'se'"
%token ENTAO            "'entao'"
%token SENAO            "'senao'"
%token FIMSE            "'fimse'"
%token ENQUANTO         "'enquanto'"
%token OU               "'ou'"
%token E                "'e'"
%token IGUAL            "'='"
%token DIFERENTE        "'!='"
%token MAIORIGUAL       "'>='"
%token MENORIGUAL       "'<='"

%token <lexeme> IDENTIFICADOR    "'identificador'"
%token <lexeme> CADEIACARACTERES "literal de 'cadeia de caracteres'"
%token <lexeme> CARCONST         "literal de 'caractere'"
%token <lexeme> INTCONST         "literal de 'inteiro'"

%type <node> Programa DeclPrograma Bloco
%type <node> DeclFunc ListaFuncoes ListaParametros ListaParametrosTail
%type <node> DeclVarGlobais VarSection ListaVar ListaDeclVar Tipo
%type <node> Comando ListaComando
%type <node> Expr LValueExpr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr ListaExpr

%%

Programa: DeclVarGlobais DeclFunc DeclPrograma {
        tree_node_set_left($2, $3);
        tree_node_set_left($1, $2);

        tree = tree_node_create(
            TREE_NODE_PROGRAMA,
            TREE_NODE_NOTYPE,
            $1,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );

        //semantic_analysis(tree);
        //generate_code(tree);
    };

DeclVarGlobais: GLOBAL VarSection {
        tree = tree_node_create(
            TREE_NODE_DECL_VAR_GLOBAIS,
            TREE_NODE_NOTYPE,
            NULL,
            $2,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | %empty {
        $$ = NULL;
    }

VarSection: '[' ListaDeclVar ']' {
        $$ = tree_node_create(
            TREE_NODE_VAR_SECTION,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };
ListaDeclVar: ListaVar ':' Tipo ';' ListaDeclVar {
        TreeNodeDataType type = tree_node_get_data_type($3);
        $$ = tree_node_create(
            TREE_NODE_LISTA_DECL_VAR,
            type,
            $1,
            $5,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | ListaVar ':' Tipo ';' {
        TreeNodeDataType type = tree_node_get_data_type($3);

        $$ = tree_node_create(
            TREE_NODE_LISTA_DECL_VAR,
            type,
            $1,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

ListaVar: IDENTIFICADOR ',' ListaVar {
        $$ = tree_node_create(
            TREE_NODE_LISTA_VAR,
            TREE_NODE_NOTYPE,
            NULL,
            $3,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR '[' INTCONST ']' ',' ListaVar {
        TreeNode* index = tree_node_create(
            TREE_NODE_INTCONST,
            TREE_NODE_INTEGER,
            NULL,
            NULL,
            $3,
            @3.first_line,
            @3.first_column
        );

        $$ = tree_node_create(
            TREE_NODE_LISTA_VAR,
            TREE_NODE_NOTYPE,
            index,
            $6,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR {
       $$ = tree_node_create(
            TREE_NODE_LISTA_VAR,
            TREE_NODE_NOTYPE,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        ); 
    }
    | IDENTIFICADOR '[' INTCONST ']' {
        TreeNode* index = tree_node_create(
            TREE_NODE_INTCONST,
            TREE_NODE_INTEGER,
            NULL,
            NULL,
            $3,
            @3.first_line,
            @3.first_column
        );

        $$ = tree_node_create(
            TREE_NODE_LISTA_VAR,
            TREE_NODE_NOTYPE,
            index,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    };

DeclFunc: FUNCAO '[' IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes ']' {
        TreeNodeDataType type = tree_node_get_data_type($8);

        TreeNode* function = tree_node_create(
            TREE_NODE_FUNCAO,
            type,
            $5,
            $9,
            $3,
            @3.first_line,
            @3.first_column
        );

        TreeNode* list_function = tree_node_create(
            TREE_NODE_LISTA_FUNCOES,
            TREE_NODE_NOTYPE,
            function,
            $10,
            NULL,
            @10.first_line,
            @10.first_column
        );

        $$ = tree_node_create(
            TREE_NODE_DECL_FUNC,
            TREE_NODE_NOTYPE,
            list_function,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | %empty {
        $$ = NULL;
    }

ListaFuncoes: IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes {
        TreeNodeDataType type = tree_node_get_data_type($6);

        TreeNode* function = tree_node_create(
            TREE_NODE_FUNCAO,
            type,
            $3,
            $7,
            $1,
            @1.first_line,
            @1.first_column
        );

        $$ = tree_node_create(
            TREE_NODE_LISTA_FUNCOES,
            TREE_NODE_NOTYPE,
            function,
            $8,
            NULL,
            @8.first_line,
            @8.first_column
        );
    }
    | %empty {
        $$ = NULL;
    };

ListaParametros: ListaParametrosTail {
        $$ = tree_node_create(
            TREE_NODE_LISTA_FUNCOES,
            TREE_NODE_NOTYPE,
            $1,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }  
    | %empty {
        $$ = NULL;
    };

ListaParametrosTail: IDENTIFICADOR ':' Tipo {
        TreeNodeDataType type = tree_node_get_data_type($3);

        $$ = tree_node_create(
            TREE_NODE_LISTA_PARAMETROS_TAIL,
            type,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        ); 
    }
    | IDENTIFICADOR '[' ']' ':' Tipo {
        TreeNodeDataType type = tree_node_get_data_type($5);

        if (type == TREE_NODE_INTEGER) {
            type =  TREE_NODE_VECTOR_INTEGER;
        } else {
            type = TREE_NODE_VECTOR_CHARACTER;
        }

        $$ = tree_node_create(
            TREE_NODE_LISTA_PARAMETROS_TAIL,
            type,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        ); 
    }
    | IDENTIFICADOR ':' Tipo ',' ListaParametrosTail {
        TreeNodeDataType type = tree_node_get_data_type($3);

        $$ = tree_node_create(
            TREE_NODE_LISTA_PARAMETROS_TAIL,
            type,
            NULL,
            $5,
            $1,
            @1.first_line,
            @1.first_column
        ); 
    }
    | IDENTIFICADOR '[' ']' ':' Tipo ',' ListaParametrosTail {
        TreeNodeDataType type = tree_node_get_data_type($5);

        if (type == TREE_NODE_INTEGER) {
            type =  TREE_NODE_VECTOR_INTEGER;
        } else {
            type = TREE_NODE_VECTOR_CHARACTER;
        }

        $$ = tree_node_create(
            TREE_NODE_LISTA_PARAMETROS_TAIL,
            type,
            NULL,
            $7,
            $1,
            @1.first_line,
            @1.first_column
        ); 
    };

DeclPrograma: PRINCIPAL Bloco {
        $$ = tree_node_create(
            TREE_NODE_DECL_PROGRAMA,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

Bloco: '{' ListaComando '}' {
        $$ = tree_node_create(
            TREE_NODE_BLOCO,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | VarSection '{' ListaComando '}' {
        $$ = tree_node_create(
            TREE_NODE_VAR_SECTION_BLOCO,
            TREE_NODE_NOTYPE,
            $3,
            $1,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

Tipo: INT {
        $$ = tree_node_create(
            TREE_NODE_INT,
            TREE_NODE_INTEGER,
            NULL,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | CAR {
        $$ = tree_node_create(
            TREE_NODE_CAR,
            TREE_NODE_CHARACTER,
            NULL,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

ListaComando: Comando {
        $$ = tree_node_create(
            TREE_NODE_LISTA_COMANDO,
            TREE_NODE_NOTYPE,
            $1,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | Comando ListaComando {
        $$ = tree_node_create(
            TREE_NODE_LISTA_COMANDO,
            TREE_NODE_NOTYPE,
            $1,
            $2,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

Comando: ';' {
        $$ = NULL;
    }
    | Expr ';' {
        $$ = $1;
    }
    | RETORNE Expr ';' {
        $$ = tree_node_create(
            TREE_NODE_RETORNE,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        ); 
    }
    | LEIA LValueExpr ';' {
        $$ = tree_node_create(
            TREE_NODE_LEIA,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        ); 
    }
    | ESCREVA Expr ';' {
        $$ = tree_node_create(
            TREE_NODE_ESCREVA_EXPRESSAO,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | ESCREVA CADEIACARACTERES ';' {
        $$ = tree_node_create(
            TREE_NODE_ESCREVA_CADEIACARACTERES,
            TREE_NODE_STRING,
            NULL,
            NULL,
            $2,
            @1.first_line,
            @1.first_column
        );
    }
    | NOVALINHA ';' {
        $$ = tree_node_create(
            TREE_NODE_NOVALINHA,
            TREE_NODE_NOTYPE,
            NULL,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | SE '(' Expr ')' ENTAO Comando FIMSE {
        $$ = tree_node_create(
            TREE_NODE_SE_ENTAO,
            TREE_NODE_NOTYPE,
            $3,
            $6,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE {
        TreeNode* node = tree_node_create(
            TREE_NODE_ENTAO_SENAO,
            TREE_NODE_NOTYPE,
            $6,
            $8,
            NULL,
            @1.first_line,
            @1.first_column
        );

        $$ = tree_node_create(
            TREE_NODE_SE_ENTAO_SENAO,
            TREE_NODE_NOTYPE,
            $3,
            node,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | ENQUANTO '(' Expr ')' Comando {
        $$ = tree_node_create(
            TREE_NODE_ENQUANTO,
            TREE_NODE_NOTYPE,
            $3,
            $5,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | Bloco {
        $$ = $1;
    };

Expr: OrExpr {
        $$ = $1;
    }
    | LValueExpr '=' Expr {
        $$  = tree_node_create(
            TREE_NODE_ASSIGN_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    };

LValueExpr: IDENTIFICADOR '[' Expr ']' {
        $$  = tree_node_create(
            TREE_NODE_IDENTIFICADOR_VARIAVEL,
            TREE_NODE_NOTYPE,
            $3,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR {
        $$  = tree_node_create(
            TREE_NODE_IDENTIFICADOR_VARIAVEL,
            TREE_NODE_NOTYPE,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    };

OrExpr: OrExpr OU AndExpr {
        $$ = tree_node_create(
            TREE_NODE_OR_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | AndExpr {
        $$ = $1;
    };

AndExpr: AndExpr E EqExpr {
        $$ = tree_node_create(
            TREE_NODE_AND_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | EqExpr {
        $$ = $1;
    };

EqExpr: EqExpr IGUAL DesigExpr {
        $$ = tree_node_create(
            TREE_NODE_EQ_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | EqExpr DIFERENTE DesigExpr {
        $$ = tree_node_create(
            TREE_NODE_NEQ_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | DesigExpr {
        $$ = $1;
    };

DesigExpr: DesigExpr '<' AddExpr {
        $$ = tree_node_create(
            TREE_NODE_LE_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | DesigExpr '>' AddExpr {
        $$ = tree_node_create(
            TREE_NODE_GE_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | DesigExpr MENORIGUAL AddExpr {
        $$ = tree_node_create(
            TREE_NODE_LEQ_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | DesigExpr MAIORIGUAL AddExpr {
        $$ = tree_node_create(
            TREE_NODE_GEQ_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | AddExpr {
        $$ = $1;
    };

AddExpr: AddExpr '+' MulExpr {
        $$ = tree_node_create(
            TREE_NODE_ADD_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | AddExpr '-' MulExpr {
        $$ = tree_node_create(
            TREE_NODE_SUB_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | MulExpr {
        $$ = $1;
    };

MulExpr: MulExpr '*' UnExpr {
        $$ = tree_node_create(
            TREE_NODE_MUL_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | MulExpr '/' UnExpr {
        $$ = tree_node_create(
            TREE_NODE_DIV_EXPR,
            TREE_NODE_NOTYPE,
            $1,
            $3,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | UnExpr {
        $$ = $1;
    };

UnExpr: '-' PrimExpr {
        $$ = tree_node_create(
            TREE_NODE_MINUS_EXPR,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | '!' PrimExpr {
        $$ = tree_node_create(
            TREE_NODE_NEG_EXPR,
            TREE_NODE_NOTYPE,
            $2,
            NULL,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
    | PrimExpr {
        $$ = $1;
    };


PrimExpr: IDENTIFICADOR '(' ListaExpr ')' {
        $$ = tree_node_create(
            TREE_NODE_IDENTIFICADOR_FUNCAO,
            TREE_NODE_NOTYPE,
            $3,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR '(' ')' {
        $$ = tree_node_create(
            TREE_NODE_IDENTIFICADOR_FUNCAO,
            TREE_NODE_NOTYPE,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR '[' Expr ']' {
        $$ = tree_node_create(
            TREE_NODE_IDENTIFICADOR_VARIAVEL,
            TREE_NODE_NOTYPE,
            $3,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | IDENTIFICADOR {
        $$ = tree_node_create(
            TREE_NODE_IDENTIFICADOR_VARIAVEL,
            TREE_NODE_NOTYPE,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | CARCONST {
        $$ = tree_node_create(
            TREE_NODE_CARCONST,
            TREE_NODE_CHARACTER,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | INTCONST {
        $$ = tree_node_create(
            TREE_NODE_INTCONST,
            TREE_NODE_INTEGER,
            NULL,
            NULL,
            $1,
            @1.first_line,
            @1.first_column
        );
    }
    | '(' Expr ')' {
        $$ = $2;
    };

ListaExpr: Expr {
        $$ = $1;
    }
    | ListaExpr ',' Expr {
        $$ = tree_node_create(
            TREE_NODE_LISTA_EXPR,
            TREE_NODE_NOTYPE,
            $3,
            $1,
            NULL,
            @1.first_line,
            @1.first_column
        );
    }
%%

/**
 * @brief Print the error, free the allocated memory and exit program
 * 
 * @param error Error to be printed
 */
void yyerror(const char *error) {
    if (analysis_error) {
        fprintf(stderr, "ERRO: %s - linha: %d, coluna: %d\n", error, yylloc.first_line, yylloc.first_column);
    }
    else {
        fprintf(stderr, "ERRO: %s\n", error);
    }
    
    exit(EXIT_FAILURE);
}

/**
 * @brief Report and format syntax error for yyerror call
 *
 * @param context Error context with all information for report error
 * @return Success
 */
int yyreport_syntax_error(const yypcontext_t *context) {
    char message[512];

    enum {
        TOKENMAX = 10
    };

    yysymbol_kind_t lookahead = yypcontext_token(context);
    yysymbol_kind_t expected[TOKENMAX];

    int n = yypcontext_expected_tokens(context, expected, TOKENMAX);
    int offset = 0;

    if (n == 0) {
        if (lookahead == YYSYMBOL_YYEMPTY) {
            offset += snprintf(message + offset, sizeof(message) - offset, "Algo inesperado ocorreu");
        }
        else if (lookahead == YYSYMBOL_YYEOF) {
            offset += snprintf(message + offset, sizeof(message) - offset, "Fim de arquivo inesperado");
        }
        else {
            offset += snprintf(message + offset, sizeof(message) - offset, "Termo %s inesperado", yysymbol_name(lookahead));
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            if (i == 0) {
                offset += snprintf(message + offset, sizeof(message) - offset, "%s %s", "Espera-se", yysymbol_name(expected[i]));
            }
            else {
                offset += snprintf(message + offset, sizeof(message) - offset, "%s %s", " ou", yysymbol_name(expected[i]));
            }
        }

        if (lookahead != YYSYMBOL_YYEMPTY) {
            if (lookahead == YYSYMBOL_YYEOF) {
                offset += snprintf(message + offset, sizeof(message) - offset, " antes do fim de arquivo");
            }
            else {
                bool has_lexeme = (
                    (lookahead == YYSYMBOL_IDENTIFICADOR) ||
                    (lookahead == YYSYMBOL_CADEIACARACTERES) ||
                    (lookahead == YYSYMBOL_INTCONST) ||
                    (lookahead == YYSYMBOL_CARCONST)
                );

                if (has_lexeme) {
                    offset += snprintf(message + offset, sizeof(message) - offset, " antes de %s (valor: '%s')", yysymbol_name(lookahead), yytext);
                }
                else {
                    offset += snprintf(message + offset, sizeof(message) - offset, " antes de %s", yysymbol_name(lookahead));
                }
            }
        }
    }

    yyerror(message);

    return 0;
}