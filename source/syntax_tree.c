#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../include/memory.h"
#include "../include/syntax_tree.h"

extern char* yytext;

/**
 * @brief Abstract Syntax Tree node struct
 * 
 */
struct _TreeNode {
    TreeNodeKind kind;
    TreeNodeDataType type;
    TreeNode* left;
    TreeNode* right;
    const char* lexeme;
    int line;
    int column;
};

/**
 * @brief Create a tree node object for build Abstract Syntax Tree
 * 
 * @param kind Kind of operation represented by the node
 * @param type Type of data lexeme or ID represented by the node
 * @param left Left child of this node
 * @param right Right child of this node
 * @param lexeme Lexeme for ID or literal data
 * @param line Line where token is in the code
 * @param column Column where token is in the code
 * @return TreeNode* A node for build Abstracted Syntax Tree
 */
TreeNode* tree_node_create(
    TreeNodeKind kind,
    TreeNodeDataType type,
    TreeNode* left,
    TreeNode* right,
    const char* lexeme,
    int line,
    int column
) {
    TreeNode* node = (TreeNode*) allocate_memory(sizeof(TreeNode));

    *node = (TreeNode){
        .kind = kind,
        .type = type,
        .left = left,
        .right = right,
        .lexeme = lexeme,
        .line = line,
        .column = column,
    };

    // printf("(%d, %d): %s\n", line, column, opa(kind));

    return node;
}

/**
 * @brief Get node kind
 *
 * @param node Tree node
 * @return Node kind
 */
TreeNodeKind tree_node_get_kind(TreeNode* node) {
    if (node == NULL) {
        return TREE_NODE_NOKIND;
    }

    return node->kind;
}

/**
 * @brief Get node data type
 *
 * @param node Tree node
 * @return Node data type
 */
TreeNodeDataType tree_node_get_data_type(TreeNode* node) {
    if (node == NULL) {
        return TREE_NODE_NOTYPE;
    }

    return node->type;
}

/**
 * @brief Get node left
 *
 * @param node Tree node
 * @return Node left
 */
TreeNode* tree_node_get_left(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->left;
}

/**
 * @brief Get node right
 *
 * @param node Tree node
 * @return Node right
 */
TreeNode* tree_node_get_right(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->right;
}

/**
 * @brief Get node line
 *
 * @param node Tree node
 * @return Node line
 */
int tree_node_get_line(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    return node->line;
}

/**
 * @brief Get node column
 *
 * @param node Tree node
 * @return Node line
 */
int tree_node_get_column(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    return node->column;
}

/**
 * @brief Get node lexeme
 *
 * @param node Tree node
 * @return Node lexeme
 */
const char* tree_node_get_lexeme(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }

    return node->lexeme;
}

/**
 * @brief Set node type
 *
 * @param node Tree node
 */
void tree_node_set_type(TreeNode* node, TreeNodeDataType type) {
    if (node == NULL) {
        return;
    }

    node->type = type;
}

/**
 * @brief Set left node
 * 
 * @param node Tree node
 * @param left Left of the tree node
 */
void tree_node_set_left(TreeNode* node, TreeNode* left) {
    if (node == NULL) {
        return;
    }

    node->left = left;
}

/**
 * @brief Set right node
 * 
 * @param node Tree node
 * @param right Right of the tree node
 */
void tree_node_set_right(TreeNode* node, TreeNode* right) {
    if (node == NULL) {
        return;
    }

    node->right = right;
}