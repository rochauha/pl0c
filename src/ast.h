/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef AST_H
#define AST_H

#include "token.h"

typedef struct ast_node {
	token_t token;
	struct ast_node *first_child;
	struct ast_node *next_sibling;
} ast_node_t;


ast_node_t *new_ast_node(token_t token);

void append_child(ast_node_t *parent, ast_node_t *new_child);

void print_ast_node(ast_node_t *node);

void print_ast(ast_node_t *root);

#endif