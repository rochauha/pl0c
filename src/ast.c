/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#include <stdio.h>
#include <stdlib.h>
#include "ast.h"


ast_node_t *new_ast_node(token_t token)
{
	ast_node_t *new_node = calloc(sizeof(ast_node_t), 1);
	new_node->token = token;
	new_node->first_child = NULL;
	new_node->next_sibling = NULL;
	return new_node;
}


void append_child(ast_node_t *parent, ast_node_t *new_child)
{
	if (!parent->first_child) {
		parent->first_child = new_child;
	}
	
	else {
		ast_node_t *current_child = parent->first_child;
		while (current_child->next_sibling) {
			current_child++;
		}
		current_child->next_sibling = new_child;
	}
}


void print_ast_node(ast_node_t *node)
{
	printf("node token:\n");
	print_token(node->token);
	printf("\n\n");
}


/* Printing AST nodes in preorder sequence */
void print_ast(ast_node_t *root)
{
	if (!root) {
		return;
	}
	else {
		print_ast_node(root);
		ast_node_t *c_root = root->first_child;
		while (c_root) {
			print_ast(c_root);
			c_root = c_root->next_sibling;
		}
	}
}