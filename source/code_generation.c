#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../include/memory.h"
#include "../include/syntax_tree.h"
#include "../include/symbol_table.h"
#include "../include/code_generation.h"

extern void yyerror(const char*);
extern bool analysis_error;
extern char* output_file;

typedef struct _Strings Strings;

Strings* strings_create(void);
void strings_delete(Strings*);
Strings* strings_add(Strings*, const char*);
int strings_get_index(Strings*, const char*);
bool strings_check(Strings*, const char*);

int get_index(void);
void emit(FILE*, const char*, ...);
void emitb(FILE*, const char*, ...);
Strings* get_strings(TreeNode*, Strings*);
Strings* generate_strings(FILE*, TreeNode*);
void build_instructions(FILE*, TreeNode*, SymbolScope*, Strings*);
void generate_instructions(FILE*, TreeNode*, Strings*);
void generate_code(TreeNode*);

/**
 * @brief Strings struct for store all
 * string literals of the code
 * 
 */
struct _Strings {
    const char* string;
    int index;
    Strings* next;
};

/**
 * @brief Create a new strings list object
 * 
 * @return Strings* String list object
 */
Strings* strings_create(void) {
    Strings* strings = (Strings*) allocate_memory(sizeof(Strings));

    *strings = (Strings) {
        .string = NULL,
        .index = 0,
        .next = NULL,
    };

    return strings;
}

/**
 * @brief Delete a strings list
 * 
 * @param strings Strings list to be deleted
 */
void strings_delete(Strings* strings) {
    while (strings != NULL) {
        Strings* next = strings->next;
        free_memory(strings);
        strings = next;
    }
}

/**
 * @brief Add a new string in the strings list
 * 
 * @param strings Strings list
 * @param string String to be added
 * @return Strings* Strings list updated
 */
Strings* strings_add(Strings* strings, const char* string) {
    if (strings_check(strings, string)) {
        return strings;
    }

    Strings* new_strings = (Strings*) allocate_memory(sizeof(Strings));

    *new_strings = (Strings) {
        .string = string,
        .index = get_index(),
        .next = strings,
    };

    return new_strings;
}

/**
 * @brief Get index of the string in strings
 * 
 * @param strings Strings list
 * @param string String to be searched
 * @return int String index
 */
int strings_get_index(Strings* strings, const char* string) {
    if (strings == NULL || string == NULL) {
        return 0;
    }

    while (strings != NULL) {
        if (strings->string == NULL) {
            break;
        }

        if (strcmp(strings->string, string) == 0) {
            return strings->index;
        }

        strings = strings->next;
    }

    return 0;
}

/**
 * @brief Check if a string already be in the strings list
 * 
 * @param strings Strings list
 * @param string String literal
 * @return true If the string already be in the strings list
 * @return false If not
 */
bool strings_check(Strings* strings, const char* string) {
    if (strings == NULL || string == NULL) {
        return true;
    }

    while (strings != NULL) {
        if (strings->string == NULL) {
            break;
        }

        if (strcmp(strings->string, string) == 0) {
            return true;
        }

        strings = strings->next;
    }

    return false;
}

/**
 * @brief Get the index value
 * 
 * @return int index value
 */
int get_index(void) {
    static int index = 1;
    return index++;
}

/**
 * @brief Write in the output file the formated text (MIPS assembly code) with
 * a tab in each line of code
 * 
 * @param file File to be wrote
 * @param format String format for the text (code)
 * @param ... The parameters for the format
 */
void emit(FILE* file, const char* format, ...) {
    if (file == NULL) {
        return;
    }

    va_list args;
    va_start(args, format);
    
    fprintf(file, "\t");
    vfprintf(file, format, args);
    fprintf(file, "\n");

    va_end(args);
}

/**
 * @brief Write in the output file the formated text (MIPS assembly code) without
 * a tab in each line of code
 * 
 * @param file File to be wrote
 * @param format String format for the text (code)
 * @param ... The parameters for the format
 */
void emitb(FILE* file, const char* format, ...) {
    if (file == NULL) {
        return;
    }

    va_list args;
    va_start(args, format);
    
    vfprintf(file, format, args);
    fprintf(file, "\n");

    va_end(args);
}

/**
 * @brief Get the strings object present in the code
 * 
 * @param node Node of the AST
 * @param strings Strings list
 * @return Strings* Updated strings list
 */
Strings* get_strings(TreeNode* node, Strings* strings) {
    if (node != NULL) {
        if (tree_node_get_kind(node) == TREE_NODE_ESCREVA_CADEIACARACTERES) {
            strings = strings_add(strings, tree_node_get_lexeme(node));
        }
        else {
            strings = get_strings(tree_node_get_left(node), strings);
            strings = get_strings(tree_node_get_right(node), strings);
        }
    }

    return strings;
}

/**
 * @brief Generate code for store all strings in memory
 * 
 * @param file Output file where code will be wrote
 * @param node Node of the AST to generate code
 * @return Strings* Strings list with all strings in the code
 */
Strings* generate_strings(FILE* file, TreeNode* node) {
    Strings* strings = strings_create();
    strings = get_strings(node, strings);
    Strings* string = strings;

    while (string != NULL) {
        if (string->string == NULL) {
            break;
        }

        emit(file, "string_%d: .asciiz \"%s\"", string->index, string->string);
        string = string->next;
    }

    emitb(file, "");

    return strings;
}


/**
 * @brief Build the instructions code in MIPS assembly
 * 
 * @param file File where the code will be wrote
 * @param node Tree node to be processed
 * @param scopes Symbol scopes
 * @param strings Strings list of strings literal
 */
void build_instructions(FILE* file, TreeNode* node, SymbolScope* scopes, Strings* strings) {
    if (node == NULL) {
        return;
    }

    TreeNodeKind kind = tree_node_get_kind(node);

    switch (kind) {
        case TREE_NODE_PROGRAMA:
            build_instructions(file, tree_node_get_left(node), scopes, strings);
            break;

        case TREE_NODE_DECL_PROGRAMA:
            build_instructions(file, tree_node_get_left(node), scopes, strings);
            break;

        case TREE_NODE_BLOCO:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
            }
            break;

        case TREE_NODE_VAR_SECTION_BLOCO:
            {
                scopes = symbol_scope_push_scope(scopes);
                build_instructions(file, tree_node_get_right(node), scopes, strings);

                int size = symbol_scope_get_size(scopes);
                emit(file, "addiu $sp, $sp, %d\n", -4*size);

                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, %d\n", 4*size);
                scopes = symbol_scope_pop_scope(scopes);
            }
            break;

        case TREE_NODE_VAR_SECTION:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
            }
            break;

        case TREE_NODE_LISTA_DECL_VAR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                build_instructions(file, tree_node_get_right(node), scopes, strings);
            }
            break;

        // TODO fix this
        case TREE_NODE_LISTA_VAR:
            {
                TreeNodeDataType type = tree_node_get_data_type(node);

                while (node != NULL) {
                    const char* name = tree_node_get_lexeme(node);

                    if (type == TREE_NODE_INTEGER) {
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
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                build_instructions(file, tree_node_get_right(node), scopes, strings);
            }
            break;

        case TREE_NODE_COMANDO:
            break;

        case TREE_NODE_LEIA:
            {
                SymbolEntry* symbol = symbol_scope_get_symbol(scopes, tree_node_get_lexeme(node), false);
                SymbolDataType type = symbol_entry_get_data_type(symbol);
                int position = symbol_entry_get_position(symbol);

                if (type == SYMBOL_INTEGER) {
                    emit(file, "li $v0, 5");
                    emit(file, "syscall");
                    emit(file, "sw $v0, %d($fp)\n", -4*position);
                }
                else if (type == SYMBOL_CHARACTER) {
                    emit(file, "li $v0, 12");
                    emit(file, "syscall");
                    emit(file, "sb $v0, %d($fp)\n", -4*position);
                }
            }
            break;

        case TREE_NODE_ESCREVA_EXPRESSAO:
            {
                TreeNode* expression = tree_node_get_left(node);
                build_instructions(file, expression, scopes, strings);

                TreeNodeDataType type = tree_node_get_data_type(expression);

                if (type == TREE_NODE_INTEGER) {
                    emit(file, "li $v0, 1");
                    emit(file, "move $a0, $s0");
                    emit(file, "syscall\n");
                }
                else if (type == TREE_NODE_CHARACTER) {
                    emit(file, "li $v0, 11");
                    emit(file, "move $a0, $s0");
                    emit(file, "syscall\n");
                }
            }
            break;

        case TREE_NODE_ESCREVA_CADEIACARACTERES:
            {
                int index = strings_get_index(strings, tree_node_get_lexeme(node));

                emit(file, "li $v0, 4");
                emit(file, "la $a0, string_%d", index);
                emit(file, "syscall\n");
            }
            break;

        case TREE_NODE_NOVALINHA:
            {
                emit(file, "li $v0, 11");
                emit(file, "li $a0, 10");
                emit(file, "syscall\n");
            }
            break;

        case TREE_NODE_SE_ENTAO:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);

                int index = get_index();

                emit(file, "li $t1, 1");
                emit(file, "bne $s0, $t1, fim_se_%d\n", index);

                build_instructions(file, tree_node_get_right(node), scopes, strings);

                emitb(file, "fim_se_%d:", index);
            }
            break;

        case TREE_NODE_SE_ENTAO_SENAO:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                build_instructions(file, tree_node_get_right(node), scopes, strings);
            }
            break;

        case TREE_NODE_ENTAO_SENAO:
            {
                int index = get_index();

                emit(file, "li $t1, 1");
                emit(file, "bne $s0, $t1, senao_%d\n", index);

                build_instructions(file, tree_node_get_left(node), scopes, strings);

                emit(file, "j fim_se_%d\n", index);
                emitb(file, "senao_%d:", index);

                build_instructions(file, tree_node_get_right(node), scopes, strings);

                emitb(file, "fim_se_%d:", index);
            }
            break;

        case TREE_NODE_ENQUANTO:
            {
                int index = get_index();
                emitb(file, "enquanto_%d:", index);

                build_instructions(file, tree_node_get_left(node), scopes, strings);

                emit(file, "li $t1, 1");
                emit(file, "bne $s0, $t1, fim_enquanto_%d", index);

                build_instructions(file, tree_node_get_right(node), scopes, strings);

                emit(file, "b enquanto_%d", index);
                emitb(file, "fim_enquanto_%d:", index);
            }
            break;
        
        case TREE_NODE_ASSIGN_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                
                SymbolEntry* symbol = symbol_scope_get_symbol(scopes, tree_node_get_lexeme(node), false);
                SymbolDataType type = symbol_entry_get_data_type(symbol);
                int position = symbol_entry_get_position(symbol);

                if (type == SYMBOL_INTEGER) {
                    emit(file, "sw $s0, %d($fp)\n", -4*position);
                }
                else if (type == SYMBOL_CHARACTER) {
                    emit(file, "sb $s0, %d($fp)\n", -4*position);
                }
            }
            break;

        case TREE_NODE_OR_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");
                
                emit(file, "or $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_AND_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "and $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_EQ_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "seq $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_NEQ_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "sne $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_LE_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "slt $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_GE_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "sgt $s0, $t1, $s0\n");
            }
            break;
            
        case TREE_NODE_LEQ_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "sle $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_GEQ_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "sge $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_ADD_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "add $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_SUB_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "sub $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_MUL_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "mul $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_DIV_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "addiu $sp, $sp, -4");
                emit(file, "sw $s0, 0($sp)");
                
                build_instructions(file, tree_node_get_right(node), scopes, strings);
                emit(file, "lw $t1, 0($sp)");
                emit(file, "addiu $sp, $sp, 4");

                emit(file, "div $s0, $t1, $s0\n");
            }
            break;

        case TREE_NODE_MINUS_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "neg $s0, $s0\n");
            }
            break;

        case TREE_NODE_NEG_EXPR:
            {
                build_instructions(file, tree_node_get_left(node), scopes, strings);
                emit(file, "seq $s0, $s0, 0\n");
            }
            break;

        // TODO: fix this
        case TREE_NODE_IDENTIFICADOR_VARIAVEL:
            {
                SymbolEntry* symbol = symbol_scope_get_symbol(scopes, tree_node_get_lexeme(node), false);
                SymbolDataType type = symbol_entry_get_data_type(symbol);
                int position = symbol_entry_get_position(symbol);

                if (type == SYMBOL_INTEGER) {
                    emit(file, "lw $s0, %d($fp)\n", -4*position);
                }
                else if (type == SYMBOL_CHARACTER) {
                    emit(file, "lb $s0, %d($fp)\n", -4*position);
                }
            }

            break;

        case TREE_NODE_CARCONST:
            {
                emit(file, "li $s0, '%s'\n", tree_node_get_lexeme(node));
            }
            break;

        case TREE_NODE_INTCONST:
            {
                emit(file, "li $s0, %s\n", tree_node_get_lexeme(node));
            }
            break;

        case TREE_NODE_NOKIND:
            break;

        default:
            break;
    }
}

/**
 * @brief Generate code with all instructions of the code
 * 
 * @param file Output file where code will be wrote
 * @param node Node of the AST to generate code
 * @param strings Strings list with all strings in the code
 */
void generate_instructions(FILE* file, TreeNode* node, Strings* strings) {
    SymbolScope* scopes = symbol_scope_create();
    build_instructions(file, node, scopes, strings);
    symbol_scope_delete(scopes);
}

/**
 * @brief Generate code from Abstract Syntax Tree
 * 
 * @param tree Abstract Syntax Tree where code will be generated from
 */
void generate_code(TreeNode* tree) {
    FILE* file = fopen(output_file, "w");

    if (file == NULL) {
        analysis_error = false;
        yyerror("Não foi possível gerar arquivo assembly!!!");
    }

    emitb(file, ".data");

    Strings* strings = generate_strings(file, tree);

    emitb(file, ".text");
    emitb(file, ".globl main\n");
    emitb(file, "main:");

    emit(file, "move $fp, $sp");

    generate_instructions(file, tree, strings);

    emit(file, "li $v0, 10");
    emit(file, "syscall");

    strings_delete(strings);
    fclose(file);
}