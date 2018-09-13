/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

static size_t global_node_count = 0;

// Ignore clang warnings in get_label
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wreturn-type"

static ast_label_t get_label(token_t token)
{
    switch (token.symbol) {
        case VAR:
            return AST_VAR_DECL;
        case CONST:
            return AST_CONST_DECL;
        case PROCEDURE:
            return AST_PROC_DECL;
        case CALL:
            return AST_CALL;
        case PLUS:
            return AST_ADD;
        case MINUS:
            return AST_SUB;
        case TIMES:
            return AST_MUL;
        case SLASH:
            return AST_DIV;
        case ODD:
            return AST_ODD;
        case ASSIGN:
            return AST_ASSIGN;
        case GTE:
            return AST_GTE;
        case LTE:
            return AST_LTE;
        case GREATER:
            return AST_GT;
        case LESSER:
            return AST_LT;
        case EQUAL:
            return AST_EQ;
        case NOTEQUAL:
            return AST_NEQ;
        case NUM:
            return AST_NUM;
        case IDENT:
            return AST_IDENT;
        case PRINT:
            return AST_PRINT;
        case SCAN:
            return AST_SCAN;
        case BEGIN:
            return AST_STMT_BLOCK;
        case IF:
            return AST_IF;
        case WHILE:
            return AST_WHILE;
    }
}

#pragma clang diagnostic pop

static void print_ast_label(ast_label_t label)
{
    switch (label) {
        case AST_ROOT:
            printf("AST_ROOT\n");
            break;
        case AST_VAR_DECL:
            printf("AST_VAR_DECL\n");
            break;
        case AST_CONST_DECL:
            printf("AST_CONST_DECL\n");
            break;
        case AST_PROC_DECL:
            printf("AST_PROC_DECL\n");
            break;
        case AST_CALL:
            printf("AST_CALL\n");
            break;
        case AST_ADD:
            printf("AST_ADD\n");
            break;
        case AST_SUB:
            printf("AST_SUB\n");
            break;
        case AST_MUL:
            printf("AST_MUL\n");
            break;
        case AST_DIV:
            printf("AST_DIV\n");
            break;
        case AST_ASSIGN:
            printf("AST_ASSIGN\n");
            break;
        case AST_ODD:
            printf("AST_ODD\n");
            break;
        case AST_GTE:
            printf("AST_GTE\n");
            break;
        case AST_LTE:
            printf("AST_LTE\n");
            break;
        case AST_GT:
            printf("AST_GT\n");
            break;
        case AST_LT:
            printf("AST_LT\n");
            break;
        case AST_EQ:
            printf("AST_EQ\n");
            break;
        case AST_NEQ:
            printf("AST_NEQ\n");
            break;
        case AST_NUM:
            printf("AST_NUM\n");
            break;
        case AST_IDENT:
            printf("AST_IDENT\n");
            break;
        case AST_PRINT:
            printf("AST_PRINT\n");
            break;
        case AST_SCAN:
            printf("AST_SCAN\n");
            break;
        case AST_BLOCK:
            printf("AST_BLOCK\n");
            break;
        case AST_STMT_BLOCK:
            printf("AST_STMT_BLOCK\n");
            break;
        case AST_IF:
            printf("AST_IF\n");
            break;
        case AST_WHILE:
            printf("AST_WHILE\n");
            break;
        default:
            printf("ERROR\n");
            break;
    }
}

ast_node_t* new_ast_node(token_t token)
{
    ast_node_t* new_node = calloc(1, sizeof(ast_node_t));
    new_node->label = get_label(token);
    new_node->first_child = NULL;
    new_node->next_sibling = NULL;
    strcpy(new_node->ident_name, token.value);
    new_node->num_value = token.num_value;
    global_node_count++;
    return new_node;
}

void append_child(ast_node_t* parent, ast_node_t* new_child)
{
    if (!parent->first_child) {
        parent->first_child = new_child;
    }

    else {
        ast_node_t* current_child = parent->first_child;
        while (current_child->next_sibling) {
            current_child = current_child->next_sibling;
        }
        current_child->next_sibling = new_child;
    }
}

static size_t child_count(ast_node_t* node)
{
    size_t count = 0;
    if (node->first_child) {
        ast_node_t* current_child = node->first_child;
        while (current_child) {
            count++;
            current_child = current_child->next_sibling;
        }
    }
    return count;
}

static void print_ast_node(ast_node_t* node)
{
    print_ast_label(node->label);
    printf("child_count : %zu\n", child_count(node));
    printf("\n\n");
}

// Printing AST nodes in preorder sequence
void print_ast(ast_node_t* root)
{
    if (!root) {
        return;
    }

    else {
        print_ast_node(root);
        ast_node_t* c_root = root->first_child;
        while (c_root) {
            print_ast(c_root);
            c_root = c_root->next_sibling;
        }
    }
}

size_t ast_node_count()
{
    return global_node_count;
}