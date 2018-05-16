/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
	AST_ROOT,
	
	AST_VAR_DECL, AST_CONST_DECL, AST_PROC_DECL, AST_CALL,
	
	AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_ASSIGN, AST_ODD,
	AST_GTE, AST_LTE, AST_GT, AST_LT, AST_EQ, AST_NEQ,
	
	AST_NUM, AST_IDENT,
	AST_PRINT, AST_SCAN,

	AST_BLOCK, AST_STMT_BLOCK,
	AST_IF, AST_WHILE

} ast_label_t;


typedef struct ast_node {
	ast_label_t label;
	int num_value;
	struct ast_node *first_child;
	struct ast_node *next_sibling;
} ast_node_t;


ast_node_t *new_ast_node(token_t token);

void append_child(ast_node_t *parent, ast_node_t *new_child);

void print_ast_node(ast_node_t *node);

void print_ast(ast_node_t *root);

size_t ast_node_count();

#endif
