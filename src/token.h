/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

typedef enum {
	// keywords
	CONST = 0, VAR, PROCEDURE, CALL,
	BEGIN, END, IF, ELSE, WHILE,
	ODD, ASSIGN, PRINT, SCAN,

	// operators
	PLUS, MINUS, TIMES, SLASH,
	GTE, LTE, GREATER, LESSER,
	EQUAL, NOTEQUAL,

	// other symbols
	COMMA, COLON, SEMICOLON,
	LPAREN, RPAREN,

	IDENT, NUM,
	ERROR,

	/*
	 * This will be used to mark end of the token list,
	 * and is not a part of the language.
	 */
	LIST_END
	
} token_symbol_t;


typedef struct {
	char value[32];
	int64_t num_value;
	token_symbol_t symbol;
} token_t;


void clear_token(token_t *t);

void print_token(token_t t);

void print_symbol(token_symbol_t s);


#endif