/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

/*
 * Key idea is to return AST nodes when a particular construct is parsed.
 * Error recovery is terrible as of now. Syntax errors would lead to a broken tree.
 */

#include <stdio.h>
#include "token.h"
#include "parser.h"
#include "ast.h"


static token_t *token_ptr;

static void accept();

static ast_node_t* parse_block();
static ast_node_t* parse_statement_block();
static ast_node_t* parse_statement();
static ast_node_t* parse_condition();
static ast_node_t* parse_expression();
static ast_node_t* parse_term();
static ast_node_t* parse_factor();


void set_token_ptr(token_t **t)
{
	token_ptr = *t;
}


ast_node_t *parse()
{
	ast_node_t *root = parse_block();
	root->label = AST_ROOT;
	accept(LIST_END);
	return root;
}


static void accept(token_symbol_t s) 
{
	if (token_ptr->symbol == s) {
		//print_token(*token_ptr);
		token_ptr++;
	}

	else {
		printf("error: expected symbol ");
		print_symbol(s);
		printf(" but found ");
		print_symbol(token_ptr->symbol);
		printf("\n");
		print_token(*token_ptr);
		token_ptr++;
	}
}


static ast_node_t* parse_block()
{
	token_t block_token = {"BEGIN", 0, BEGIN};
	ast_node_t *main_root = new_ast_node(block_token);
	main_root->label = AST_BLOCK;

	ast_node_t *const_decl = NULL;
	ast_node_t *var_decl = NULL;
	ast_node_t *proc_decl = NULL;
	ast_node_t *new_child = NULL;
	ast_node_t *new_child_num = NULL;
	
	if (token_ptr->symbol == CONST) {
		accept(CONST);
		const_decl = new_ast_node(*(token_ptr-1));
		accept(IDENT);
		new_child = new_ast_node(*(token_ptr-1));
		append_child(const_decl, new_child);
		accept(ASSIGN);
		accept(NUM);
		new_child_num = new_ast_node(*(token_ptr-1));
		append_child(new_child, new_child_num);

		while (token_ptr->symbol == COMMA) {
			accept(COMMA);
			accept(IDENT);
			new_child = new_ast_node(*(token_ptr-1));
			append_child(const_decl, new_child);
			accept(ASSIGN);
			accept(NUM);
			new_child_num = new_ast_node(*(token_ptr-1));
			append_child(new_child, new_child_num);
		}
		accept(SEMICOLON);
		append_child(main_root, const_decl);
	}

	if (token_ptr->symbol == VAR) {
		accept(VAR);
		var_decl = new_ast_node(*(token_ptr-1));
		accept(IDENT);
		new_child = new_ast_node(*(token_ptr-1));
		append_child(var_decl, new_child);
		while (token_ptr->symbol == COMMA) {
			accept(COMMA);
			accept(IDENT);
			new_child = new_ast_node(*(token_ptr-1));
			append_child(var_decl, new_child);
		}
		accept(SEMICOLON);
		append_child(main_root, var_decl);
	}

	while (token_ptr->symbol == PROCEDURE) {
		accept(PROCEDURE);
		proc_decl = new_ast_node(*(token_ptr-1));
		accept(IDENT);
		new_child = new_ast_node(*(token_ptr-1));
		append_child(proc_decl, new_child);
		accept(COLON);
		new_child = parse_block();
		append_child(proc_decl, new_child);
		append_child(main_root, proc_decl);
	}
	
	new_child = parse_statement_block();
	append_child(main_root, new_child);
	return main_root;
}


static ast_node_t* parse_statement_block()
{
	ast_node_t *main_root = NULL;
	ast_node_t *new_child = NULL;

	if (token_ptr->symbol == BEGIN) {
		accept(BEGIN);
		main_root = new_ast_node(*(token_ptr-1));	
		while (token_ptr->symbol == IDENT || token_ptr->symbol == CALL ||
				 token_ptr->symbol == IF || token_ptr->symbol == WHILE ||
				 token_ptr->symbol == PRINT || token_ptr->symbol == SCAN ) {
			new_child = parse_statement();
			append_child(main_root, new_child);
		}
		accept(END);
	}

	else {
		main_root = parse_statement();
	}

	return main_root;
}


static ast_node_t* parse_statement()
{
	ast_node_t *main_root = NULL;
	ast_node_t *operand = NULL;
	ast_node_t *new_child = NULL; /* Using this for better code readability */

	if (token_ptr->symbol == IDENT) {
		accept(IDENT);
		operand = new_ast_node(*(token_ptr-1));
		accept(ASSIGN);
		main_root = new_ast_node(*(token_ptr-1));
		append_child(main_root, operand);
		operand = parse_expression();
		append_child(main_root, operand);
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == CALL) {
		accept(CALL);
		main_root = new_ast_node(*(token_ptr-1));
		accept(IDENT);
		operand = new_ast_node(*(token_ptr-1));
		append_child(main_root, operand);
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == IF) {
		accept(IF);
		main_root = new_ast_node(*(token_ptr-1));
		new_child = parse_condition();
		append_child(main_root, new_child);
		accept(COLON);
		new_child = parse_statement_block();
		append_child(main_root, new_child);
		if (token_ptr->symbol == ELSE) {
			accept(ELSE);
			accept(COLON);
			new_child = parse_statement_block();
			append_child(main_root, new_child);
		}
	}

	else if (token_ptr->symbol == WHILE) {
		accept(WHILE);
		main_root = new_ast_node(*(token_ptr-1));
		new_child = parse_condition();
		append_child(main_root, new_child);
		accept(COLON);
		new_child = parse_statement_block();
		append_child(main_root, new_child);
	}

	else if (token_ptr->symbol == PRINT) {
		accept(PRINT);
		main_root = new_ast_node(*(token_ptr-1));
		if (token_ptr->symbol == IDENT) {
			accept(IDENT);
			operand = new_ast_node(*(token_ptr-1));
			append_child(main_root, operand);
		}
		else {
			accept(NUM);
			operand = new_ast_node(*(token_ptr-1));
			append_child(main_root, operand);
		}
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == SCAN) {
		accept(SCAN);
		main_root = new_ast_node(*(token_ptr-1));
		accept(IDENT);
		operand = new_ast_node(*(token_ptr-1));
		append_child(main_root, operand);
		accept(SEMICOLON);
	}

	return main_root;
}


static ast_node_t* parse_condition()
{
	ast_node_t *main_root = NULL;
	ast_node_t *operand = NULL;
	if (token_ptr->symbol == ODD) {
		accept(ODD);
		main_root = new_ast_node(*(token_ptr-1));
		operand = parse_expression();
		append_child(main_root, operand);
	}

	else {
		operand = parse_expression();

		if (token_ptr->symbol == GTE || token_ptr->symbol == LTE ||
			token_ptr->symbol == GREATER || token_ptr->symbol == LESSER ||
			token_ptr->symbol == NOTEQUAL || token_ptr->symbol == EQUAL) {
			
			main_root = new_ast_node(*token_ptr);
			token_ptr++;
			append_child(main_root, operand);
			operand = parse_expression();
			append_child(main_root, operand);
		}

		else {
			printf("error: inavid conditional operator\n");
			token_ptr++;
		}
	}

	return main_root;
}


static ast_node_t* parse_expression()
{
	ast_node_t *operand = NULL;
	ast_node_t *tmp_root = NULL;
	ast_node_t *main_root = NULL;
	ast_node_t *current_root = NULL;

	if (token_ptr->symbol == PLUS) {
		accept(PLUS);
		current_root = new_ast_node(*(token_ptr-1));
		main_root = current_root;
	}

	else if (token_ptr->symbol == MINUS) {
		accept(MINUS);
		current_root = new_ast_node(*(token_ptr-1));	
		main_root = current_root;
	}

	operand = parse_term();

	while (token_ptr->symbol == PLUS || token_ptr->symbol == MINUS) {
		if (token_ptr->symbol == PLUS) {
			accept(PLUS);
			tmp_root = current_root;
			current_root = new_ast_node(*(token_ptr-1));
			append_child(current_root, operand);
			if (tmp_root) {
				append_child(tmp_root, current_root);
				main_root = (main_root) ? main_root : tmp_root;
			}
		}

		else if (token_ptr->symbol == MINUS) {
			accept(MINUS);
			tmp_root = current_root;
			current_root = new_ast_node(*(token_ptr-1));
			append_child(current_root, operand);
			if (tmp_root) {
				append_child(tmp_root, current_root);
				main_root = (main_root) ? main_root : tmp_root;
			}
		}

		operand = parse_term();
		if (token_ptr->symbol != PLUS && token_ptr->symbol != MINUS) {
			/* Next symbol is not an operation, so append the operand to the
			 * current operation. The loop will break after this
			 */
			append_child(current_root, operand);
			main_root = (main_root) ? main_root : current_root;
		}
	}

	return (main_root) ? main_root : operand;
}


static ast_node_t* parse_term()
{
	ast_node_t *operand = parse_factor();
	ast_node_t *tmp_root = NULL;
	ast_node_t *main_root = NULL;
	ast_node_t *current_root = NULL;
	
	while (token_ptr->symbol == TIMES || token_ptr->symbol == SLASH) {
		if (token_ptr->symbol == TIMES) {
			accept(TIMES);
			tmp_root = current_root;
			current_root = new_ast_node(*(token_ptr-1));
			append_child(current_root, operand);
			if (tmp_root) {
				append_child(tmp_root, current_root);
				main_root = (main_root) ? main_root : tmp_root;
			}
		}

		else if (token_ptr->symbol == SLASH) {
			accept(SLASH);
			tmp_root = current_root;
			current_root = new_ast_node(*(token_ptr-1));
			append_child(current_root, operand);
			if (tmp_root) {
				append_child(tmp_root, current_root);
				main_root = (main_root) ? main_root : tmp_root;
			}
		}

		operand = parse_factor();
		if (token_ptr->symbol != TIMES && token_ptr->symbol != SLASH) {
			/* Next symbol is not an operation, so append the operand to the
			 * current operation. The loop will break after this
			 */
			append_child(current_root, operand);
			main_root = (main_root) ? main_root : current_root;
		}
	}

	return (main_root) ? main_root : operand;
}


static ast_node_t* parse_factor()
{
	if (token_ptr->symbol == NUM) {
		accept(NUM);
		return new_ast_node(*(token_ptr-1));
	}

	else if (token_ptr->symbol == LPAREN) {
		accept(LPAREN);
		ast_node_t *main_root = parse_expression();
		accept(RPAREN);
		return main_root;
	}

	else {
		accept(IDENT);
		return new_ast_node(*(token_ptr-1));
	}
}