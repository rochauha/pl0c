/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE.md for more details.
 */


#ifndef LEXER_C
#define LEXER_C


#define _POSIX_C_SOURCE 200809L // for using open_memstream
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"


bool valid_char(char c)
{
	bool x = isspace(c) || isalnum(c) ||
	c == '+' || c == '-' || c == '*' ||
	c == '/' || c == '=' || c == '<' ||
	c == '>' || c == ',' || c == ';' ||
	c == '?' || c == '(' || c == ')';
	return x;
}

/*
 * Scan entire source file, return number of tokens collected
 */
size_t scan(const char *source, char **buf)
{
	token_t token_holder;
	size_t init_len = 0;
	size_t n_elements = 0;

	FILE *fin = fopen(source, "r");
	
	/* file stream associated with memory buffer */
	FILE *token_stream = open_memstream(buf, &init_len);

	int i = 0;
	char c = ' ';
	
	while (c != EOF) {
		if (!valid_char(c)){
			clear_token(&token_holder);
			token_holder.symbol = ERROR;
			i = 0;
			while (c != EOF && !valid_char(c)) {
				token_holder.value[i++] = c;
				c = fgetc(fin);
			}
		}

		else if (isspace(c)) {
			while (isspace(c)) {
				c = fgetc(fin);
			}
			continue;
		}

		else if (isdigit(c)) {
			clear_token(&token_holder);
			token_holder.symbol = NUM;
			i = 0;
			while (isdigit(c)) {
				token_holder.value[i++] = c;
				token_holder.num_value *= 10;
				token_holder.num_value += (c - 48);
				c = fgetc(fin);
			}
		}

		else if (isalpha(c)) {
			clear_token(&token_holder);
			i = 0;
			token_holder.symbol = IDENT;
			while (isalnum(c)) {
				token_holder.value[i++] = c;
				c = fgetc(fin);
			}
			set_keyword(&token_holder);
		}

		else if (c == '+') {
			clear_token(&token_holder);
			token_holder.symbol = PLUS;
			token_holder.value[0] = '+';
			c = fgetc(fin);	
		}

		else if (c == '-') {
			clear_token(&token_holder);
			token_holder.symbol = MINUS;
			token_holder.value[0] = '-';
			c = fgetc(fin);			
		}

		else if (c == '*') {
			clear_token(&token_holder);
			token_holder.symbol = TIMES;
			token_holder.value[0] = '*';
			c = fgetc(fin);
		}

		else if (c == '/') {
			clear_token(&token_holder);			
			token_holder.symbol = SLASH;
			token_holder.value[0] = '/';
			c = fgetc(fin);	
		}

		else if (c == ',') {
			clear_token(&token_holder);			
			token_holder.symbol = COMMA;
			token_holder.value[0] = ',';
			c = fgetc(fin);	
		}

		else if (c == ';') {
			clear_token(&token_holder);			
			token_holder.symbol = SEMICOLON;
			token_holder.value[0] = ';';
			c = fgetc(fin);
		}

		else if (c == '!') {
			clear_token(&token_holder);			
			token_holder.symbol = EXCLAMATION;
			token_holder.value[0] = '!';
			c = fgetc(fin);

			// Check for next '=' char
			if (c == '=') {
				token_holder.symbol = NOTEQUAL;
				token_holder.value[1] = '=';
				c = fgetc(fin);
			}
		}

		else if (c == '>') {
			clear_token(&token_holder);			
			token_holder.symbol = GREATER;
			token_holder.value[0] = '>';
			c = fgetc(fin);

			// Check for next '=' char
			if (c == '=') {
				token_holder.symbol = GTE;
				token_holder.value[1] = '=';
				c = fgetc(fin);
			}
		}

		else if (c == '<') {
			clear_token(&token_holder);			
			token_holder.symbol = LESSER;
			token_holder.value[0] = '<';
			c = fgetc(fin);
	
			// Check for next '=' char
			if (c == '=') {
				token_holder.symbol = LTE;
				token_holder.value[1] = '=';
				c = fgetc(fin);
			}		
		}

		else if (c == '?') {
			clear_token(&token_holder);
			token_holder.symbol = QUESTIONMARK;
			token_holder.value[0] = '?';
			c = fgetc(fin);
		}

		else if (c == '=') {
			clear_token(&token_holder);			
			token_holder.symbol = EQUAL;
			token_holder.value[0] = '=';
			c = fgetc(fin);
		}

		else if (c == '(') {
			clear_token(&token_holder);			
			token_holder.symbol = LPAREN;
			token_holder.value[0] = '(';
			c = fgetc(fin);
		}

		else if (c == ')') {
			clear_token(&token_holder);			
			token_holder.symbol = RPAREN;
			token_holder.value[0] = ')';
			c = fgetc(fin);
		}

		/* Write token to token_stream, basically it is pushed to the
		 * dynamic memory buffer associated with token_stream
		 */
		fwrite (&token_holder, 1, sizeof(token_holder), token_stream);
		fflush(token_stream);
		n_elements++;
	}
	fclose(token_stream);
	return n_elements;
}


/* 
 * Set symbol to respective keyword if it's string matches that keyword 
 */
void set_keyword(token_t *t)
{
	if (strcmp(t->value, "CONST") == 0)
		t->symbol = CONST;

	else if (strcmp(t->value, "VAR") == 0)
		t->symbol = VAR;
	
	else if (strcmp(t->value, "PROCEDURE") == 0)
		t->symbol = PROCEDURE;
	
	else if (strcmp(t->value, "CALL") == 0)
		t->symbol = CALL;
	
	else if (strcmp(t->value, "BEGIN") == 0)
		t->symbol = BEGIN;
	
	else if (strcmp(t->value, "END") == 0)
		t->symbol = END;
	
	else if (strcmp(t->value, "IF") == 0)
		t->symbol = IF;
	
	else if (strcmp(t->value, "THEN") == 0)
		t->symbol = THEN;
	
	else if (strcmp(t->value, "WHILE") == 0)
		t->symbol = WHILE;
	
	else if (strcmp(t->value, "DO") == 0)
		t->symbol = DO;
	
	else if (strcmp(t->value, "ODD") == 0)
		t->symbol = ODD;
}


#endif