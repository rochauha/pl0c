/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef TOKEN_H
#define TOKEN_H


typedef enum {
	// keywords
	CONST = 0, VAR, PROCEDURE, CALL,
	BEGIN, END, IF, THEN, WHILE, DO,
	ODD,

	// operators
	PLUS, MINUS, TIMES, SLASH,
	GTE, LTE, GREATER, LESSER,
	EQUAL, NOTEQUAL,

	// other symbols
	COMMA, SEMICOLON, QUESTIONMARK,
	EXCLAMATION, LPAREN, RPAREN,

	IDENT, NUM,
	ERROR,

	/*
	 * This will be used to mark end of the token list,
	 * and is not a part of the language.
	 */
	LIST_END
	
} symbol_t;


typedef struct {
	char value[32];
	int num_value;
	symbol_t symbol;
} token_t;


void clear_token(token_t *t);

void print_token(token_t t);


#endif