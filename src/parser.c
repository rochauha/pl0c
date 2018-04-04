/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

/*
 * Initial parser, just does syntax analysis.
 * Error recovery is terrible as of now.
 */

#include <stdio.h>
#include "token.h"
#include "parser.h"


static token_t *token_ptr;

static void accept();

static void parse_block();
static void parse_statement_block();
static void parse_statement();
static void parse_condition();
static void parse_expression();
static void parse_term();
static void parse_factor();


void set_token_ptr(token_t **t)
{
	token_ptr = *t;
}


void parse()
{
	parse_block();
	accept(LIST_END);
}


static void accept(symbol_t s) 
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


static void parse_block()
{
	if (token_ptr->symbol == CONST) {
		accept(CONST);
		accept(IDENT);
		accept(ASSIGN);
		accept(NUM);
		while (token_ptr->symbol == COMMA) {
			accept(COMMA);
			accept(IDENT);
			accept(NUM);
		}
		accept(SEMICOLON);
	}

	if (token_ptr->symbol == VAR) {
		accept(VAR);
		accept(IDENT);
		while (token_ptr->symbol == COMMA) {
			accept(COMMA);
			accept(IDENT);
		}
		accept(SEMICOLON);
	}

	while (token_ptr->symbol == PROCEDURE) {
		accept(PROCEDURE);
		accept(IDENT);
		accept(COLON);
		parse_block();
	}

	parse_statement_block();
}


static void parse_statement_block()
{
	if (token_ptr->symbol == BEGIN) {
		accept(BEGIN);
		while (token_ptr->symbol == IDENT || token_ptr->symbol == CALL ||
				 token_ptr->symbol == IF || token_ptr->symbol == WHILE ||
				 token_ptr->symbol == PRINT || token_ptr->symbol == SCAN ) {
			parse_statement();
		}
		accept(END);
	}

	else {
		parse_statement();
	}
}


static void parse_statement()
{
	if (token_ptr->symbol == IDENT) {
		accept(IDENT);
		accept(ASSIGN);
		parse_expression();
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == CALL) {
		accept(CALL);
		accept(IDENT);
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == IF) {
		accept(IF);
		parse_condition();
		accept(COLON);
		parse_statement_block();
		if (token_ptr->symbol == ELSE) {
			accept(ELSE);
			accept(COLON);
			parse_statement_block();
		}
	}

	else if (token_ptr->symbol == WHILE) {
		accept(WHILE);
		parse_condition();
		accept(COLON);
		parse_statement_block();
	}

	else if (token_ptr->symbol == PRINT) {
		accept(PRINT);
		if (token_ptr->symbol == IDENT) {
			accept(IDENT);
		}
		else {
			accept(NUM);
		}
		accept(SEMICOLON);
	}

	else if (token_ptr->symbol == SCAN) {
		accept(SCAN);
		accept(IDENT);
		accept(SEMICOLON);
	}
}


static void parse_condition()
{
	if (token_ptr->symbol == ODD) {
		accept(ODD);
		parse_expression();
	}

	else {
		parse_expression();

		if (token_ptr->symbol == GTE || token_ptr->symbol == LTE ||
			token_ptr->symbol == GREATER || token_ptr->symbol == LESSER ||
			token_ptr->symbol == NOTEQUAL || token_ptr->symbol == EQUAL) {
			token_ptr++;
			parse_expression();
		}

		else {
			printf("error: inavid conditional operator\n");
			token_ptr++;
		}
	}
}


static void parse_expression()
{
	if (token_ptr->symbol == PLUS) {
		accept(PLUS);
	}

	else if (token_ptr->symbol == MINUS) {
		accept(MINUS);
	}

	parse_term();

	while (token_ptr->symbol == PLUS || token_ptr->symbol == MINUS) {
		if (token_ptr->symbol == PLUS) {
			accept(PLUS);
		}

		else if (token_ptr->symbol == MINUS) {
			accept(MINUS);
		}
		parse_term();
	}
}


static void parse_term()
{
	parse_factor();
	while (token_ptr->symbol == TIMES || token_ptr->symbol == SLASH) {
		if (token_ptr->symbol == TIMES) {
			accept(TIMES);
		}

		else if (token_ptr->symbol == SLASH) {
			accept(SLASH);
		}

		parse_factor();
	}
}


static void parse_factor()
{
	if (token_ptr->symbol == NUM) {
		accept(NUM);
	}

	else if (token_ptr->symbol == LPAREN) {
		accept(LPAREN);
		parse_expression();
		accept(RPAREN);
	}

	else {
		accept(IDENT);
	}
}