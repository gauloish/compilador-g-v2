#include <stdlib.h>
#include <stdio.h>

#include "../include/memory.h"
#include "../include/syntax_tree.h"
#include "../include/symbol_table.h"
#include "../include/semantic_analysis.h"

#define MESSAGE_SIZE 512

extern void yyerror(const char*);

/**
 * @brief Print the semantic error, free the allocated memory and exit program
 * 
 * @param message Error message
 * @param node Tree node where error occurs
 */
void report_semantic_error(const char* message, TreeNode* node) {
    int line = tree_node_get_line(node);
    int column = tree_node_get_column(node);

    fprintf(stderr, "ERRO: %s - LINHA: %d, COLUNA: %d\n", message, line, column);
    exit(EXIT_FAILURE);
}

/**
 * @brief Traverse the Abstract Syntax Tree doing the semantic analysis
 * 
 * @param node Current node in the AST to be analysed
 * @param scopes Symbol scope stack to help in the analysis
 */
void traverse_tree(TreeNode* node, SymbolScope* scopes) {
    static char message[MESSAGE_SIZE];

    if (node == NULL) {
        return;
    }

    TreeNodeKind kind = tree_node_get_kind(node);

    switch (kind) {
        case TREE_NODE_PROGRAMA:
            {
                traverse_tree(tree_node_get_left(node), scopes);
            }
            break;

        case TREE_NODE_DECL_PROGRAMA:
            {
                traverse_tree(tree_node_get_left(node), scopes);
            }
            break;

        case TREE_NODE_BLOCO:
            {
                traverse_tree(tree_node_get_left(node), scopes);
            }
            break;

        case TREE_NODE_VAR_SECTION_BLOCO:
            {
                scopes = symbol_scope_push_scope(scopes);
                traverse_tree(tree_node_get_right(node), scopes);
                traverse_tree(tree_node_get_left(node), scopes);
                scopes = symbol_scope_pop_scope(scopes);
            }
            break;

        case TREE_NODE_VAR_SECTION:
            {
                traverse_tree(tree_node_get_left(node), scopes);
            }
            break;

        case TREE_NODE_LISTA_DECL_VAR:
            {
                traverse_tree(tree_node_get_left(node), scopes);
                traverse_tree(tree_node_get_right(node), scopes);
            }
            break;

        // TODO: fix this
        case TREE_NODE_LISTA_VAR:
            {
                TreeNodeDataType type = tree_node_get_data_type(node);

                while (node != NULL) {
                    const char* name = tree_node_get_lexeme(node);

                    if (symbol_scope_check_symbol(scopes, name, true)) {
                        snprintf(message, sizeof(message), "Variável '%s' já declarada nesse escopo", name);
                        report_semantic_error(message, node);
                    }
                    else if (type == TREE_NODE_INTEGER) {
                        symbol_scope_add_symbol(scopes, name, SYMBOL_INTEGER);
                    }
                    else if (type == TREE_NODE_CHARACTER) {
                        symbol_scope_add_symbol(scopes, name, SYMBOL_CHARACTER);
                    }

                    tree_node_set_type(node, type);
                    node = tree_node_get_left(node);
                }
            }

            break;

        case TREE_NODE_INT:
            break;

        case TREE_NODE_CAR:
            break;

        case TREE_NODE_LISTA_COMANDO:
            {
                traverse_tree(tree_node_get_left(node), scopes);
                traverse_tree(tree_node_get_right(node), scopes);
            }
            break;

        case TREE_NODE_COMANDO:
            break;

        case TREE_NODE_LEIA:
            {
                const char* name = tree_node_get_lexeme(node);

                if (!symbol_scope_check_symbol(scopes, name, false)) {
                    snprintf(message, sizeof(message), "Variável '%s' não declarada previamente", name);
                    report_semantic_error(message, node);
                }
            }

            break;

        case TREE_NODE_ESCREVA_EXPRESSAO:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                TreeNodeDataType type = tree_node_get_data_type(expression);

                if (type != TREE_NODE_INTEGER && type != TREE_NODE_CHARACTER) {
                    snprintf(message, sizeof(message), "Expressão deve ter valor do tipo 'inteiro' ou 'caractere' para ser impressa");
                    report_semantic_error(message, expression);
                }
            }

            break;

        case TREE_NODE_ESCREVA_CADEIACARACTERES:
            break;

        case TREE_NODE_NOVALINHA:
            break;

        case TREE_NODE_SE_ENTAO:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                if (tree_node_get_data_type(expression) != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "Expressão condicional deve ter valor lógico");
                    report_semantic_error(message, expression);
                }

                traverse_tree(tree_node_get_right(node), scopes);
            }

            break;

        case TREE_NODE_SE_ENTAO_SENAO:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                if (tree_node_get_data_type(expression) != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "Expressão condicional deve ter valor lógico");
                    report_semantic_error(message, expression);
                }

                traverse_tree(tree_node_get_right(node), scopes);
            }

            break;

        case TREE_NODE_ENTAO_SENAO:
            {
                traverse_tree(tree_node_get_left(node), scopes);
                traverse_tree(tree_node_get_right(node), scopes);
            }
            break;

        case TREE_NODE_ENQUANTO:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                if (tree_node_get_data_type(expression) != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "Expressão condicional deve ter valor lógico");
                    report_semantic_error(message, expression);
                }

                traverse_tree(tree_node_get_right(node), scopes);
            }
            break;
        
        case TREE_NODE_ASSIGN_EXPR:
            {
                const char* name = tree_node_get_lexeme(node);

                if (!symbol_scope_check_symbol(scopes, name, false)) {
                    snprintf(message, sizeof(message), "Variável '%s' não declarada previamente", name);
                    report_semantic_error(message, node);
                }

                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                SymbolEntry* symbol = symbol_scope_get_symbol(scopes, name, false);
                SymbolDataType type = symbol_entry_get_data_type(symbol);

                if (type == SYMBOL_INTEGER) {
                    if (tree_node_get_data_type(expression) != TREE_NODE_INTEGER) {
                        snprintf(message, sizeof(message), "O valor da expressão deve ser do tipo 'inteiro' para ser atribuída à variável '%s'", name);
                        report_semantic_error(message, expression);
                    }
                }
                else if (type == SYMBOL_CHARACTER) {
                    if (tree_node_get_data_type(expression) != TREE_NODE_CHARACTER) {
                        snprintf(message, sizeof(message), "O valor da expressão deve ser do tipo 'caractere' para ser atribuída à variável '%s'", name);
                        report_semantic_error(message, expression);
                    }
                }
                else {
                    snprintf(message, sizeof(message), "A atribuição de valores só pode ser feito para valores dos tipos 'inteiro' ou 'caractere'");
                    report_semantic_error(message, node);
                }
            }

            break;

        case TREE_NODE_OR_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor lógico");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor lógico");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_AND_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor lógico");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor lógico");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_EQ_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type == TREE_NODE_STRING || left_type == TREE_NODE_NOTYPE) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor dos tipos 'inteiro' ou 'caractere'");
                    report_semantic_error(message, left);
                }
                else if (right_type == TREE_NODE_STRING || right_type == TREE_NODE_NOTYPE) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor dos tipos 'inteiro' ou 'caractere'");
                    report_semantic_error(message, right);
                }
                else if (left_type != right_type) {
                    snprintf(message, sizeof(message), "Ambos os operandos devem ter valores dos mesmos tipos");
                    report_semantic_error(message, node);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_NEQ_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type == TREE_NODE_STRING || left_type == TREE_NODE_NOTYPE) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor dos tipos 'inteiro' ou 'caractere'");
                    report_semantic_error(message, left);
                }
                else if (right_type == TREE_NODE_STRING || right_type == TREE_NODE_NOTYPE) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor dos tipos 'inteiro' ou 'caractere'");
                    report_semantic_error(message, right);
                }
                else if (left_type != right_type) {
                    snprintf(message, sizeof(message), "Ambos os operandos devem ter valores dos mesmos tipos");
                    report_semantic_error(message, node);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_LE_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_GE_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;
            
        case TREE_NODE_LEQ_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_GEQ_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        case TREE_NODE_ADD_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
            }

            break;

        case TREE_NODE_SUB_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
            }

            break;

        case TREE_NODE_MUL_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
            }

            break;

        case TREE_NODE_DIV_EXPR:
            {
                TreeNode* left = tree_node_get_left(node);
                TreeNode* right = tree_node_get_right(node);

                traverse_tree(left, scopes);
                traverse_tree(right, scopes);

                TreeNodeDataType left_type = tree_node_get_data_type(left);
                TreeNodeDataType right_type = tree_node_get_data_type(right);

                if (left_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O primeiro operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, left);
                }
                else if (right_type != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O segundo operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, right);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
            }

            break;

        case TREE_NODE_MINUS_EXPR:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                if (tree_node_get_data_type(expression) != TREE_NODE_INTEGER) {
                    snprintf(message, sizeof(message), "O operando deve ter valor do tipo 'inteiro'");
                    report_semantic_error(message, expression);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
            }

            break;

        case TREE_NODE_NEG_EXPR:
            {
                TreeNode* expression = tree_node_get_left(node);
                traverse_tree(expression, scopes);

                if (tree_node_get_data_type(expression) != TREE_NODE_BOOLEAN) {
                    snprintf(message, sizeof(message), "O operando deve ter valor lógico");
                    report_semantic_error(message, expression);
                }
                else {
                    tree_node_set_type(node, TREE_NODE_BOOLEAN);
                }
            }

            break;

        // TODO: fix this
        case TREE_NODE_IDENTIFICADOR_FUNCAO:
            {
                const char* name = tree_node_get_lexeme(node);

                if (!symbol_scope_check_symbol(scopes, name, false)) {
                    snprintf(message, sizeof(message), "Variável '%s' não declarada previamente", name);
                    report_semantic_error(message, node);
                }

                SymbolEntry* symbol = symbol_scope_get_symbol(scopes, name, false);
                SymbolDataType type = symbol_entry_get_data_type(symbol);

                if (type == SYMBOL_INTEGER) {
                    tree_node_set_type(node, TREE_NODE_INTEGER);
                }
                else if (type == SYMBOL_CHARACTER) {
                    tree_node_set_type(node, TREE_NODE_CHARACTER);
                }
                else {
                    snprintf(message, sizeof(message), "A variável '%s' devem ser do tipo 'inteiro' ou 'caractere'", name);
                    report_semantic_error(message, node);
                }
            }

            break;

        case TREE_NODE_CARCONST:
            break;

        case TREE_NODE_INTCONST:
            // TODO: Check if int const fit in a 32-bit integer
            break;

        case TREE_NODE_NOKIND:
            {
                snprintf(message, sizeof(message), "Algo inesperado ocorreu");
                report_semantic_error(message, node);
            }
            break;

        default:
            break;
    }
}

/**
 * @brief Perform semantic analysis verifying variable scope, declaration
 * and duplication, type checking and preprocessing AST for code generation.
 * 
 * @param node Node of AST to be analysed by semantic analyser
 */
void semantic_analysis(TreeNode* node) {
    if (node == NULL) {
        return;
    }

    SymbolScope* scopes = symbol_scope_create();

    traverse_tree(node, scopes);
    symbol_scope_delete(scopes);
}