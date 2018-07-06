/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#define _POSIX_C_SOURCE 200809L // for using open_memstream
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "lexer.h"


static bool valid_char(char c);
static void set_keyword(token_t *t);


static bool valid_char(char c)
{
	return isspace(c) || isalnum(c) ||
	c == '+' || c == '-' || c == '*' ||
	c == '/' || c == '=' || c == '<' ||
	c == '>' || c == ',' || c == ':' ||
	c == ';' || c == '_' || c == '!' ||
	c == '(' || c == ')';
}


/*
 * Scan entire source file
 */
void scan(const char *source, char **buf)
{
	token_t token_holder;
	size_t init_len = 0;
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

		else if (isalpha(c) || c == '_') {
			clear_token(&token_holder);
			i = 0;
			token_holder.symbol = IDENT;
			while (isalnum(c) || c == '_') {
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

		else if (c == ':') {
			clear_token(&token_holder);			
			token_holder.symbol = COLON;
			token_holder.value[0] = ':';
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
			token_holder.value[0] = '!';
			c = fgetc(fin);	
			// Check for next '=' char
			if (c == '=') {
				token_holder.symbol = NOTEQUAL;
				token_holder.value[1] = '=';
				c = fgetc(fin);
			}
			else {
				token_holder.symbol = ERROR;
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

		else if (c == '=') {
			clear_token(&token_holder);			
			token_holder.symbol = ASSIGN;
			token_holder.value[0] = '=';
			c = fgetc(fin);
			// Check for next '=' char
			if (c == '=') {
				token_holder.symbol = EQUAL;
				token_holder.value[1] = '=';
				c = fgetc(fin);
			}
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

		/* 
		 * Write token to token_stream, basically it is pushed to the
		 * dynamic memory buffer associated with token_stream
		 */
		fwrite (&token_holder, 1, sizeof(token_holder), token_stream);
		fflush(token_stream);
	}

	clear_token(&token_holder);
	token_holder.symbol = LIST_END;
	fwrite (&token_holder, 1, sizeof(token_holder), token_stream);
	fflush(token_stream);
	fclose(token_stream);
}


/* 
 * Set symbol to respective keyword if it's string matches that keyword 
 */
static void set_keyword(token_t *t)
{
	/* 
	 * Making keywords case insensitive. 
	 * The original string associated with the token is not affected 
	 */
	char string[32];
	char *c_ptr = string;
	
	strcpy(string, t->value);

	while (*c_ptr) {
		*c_ptr = toupper(*c_ptr);
		c_ptr++;
	}
	
	if (strcmp(string, "CONST") == 0) t->symbol = CONST;
	else if (strcmp(string, "VAR") == 0) t->symbol = VAR;
	else if (strcmp(string, "PROCEDURE") == 0) t->symbol = PROCEDURE;
	else if (strcmp(string, "CALL") == 0) t->symbol = CALL;
	else if (strcmp(string, "BEGIN") == 0) t->symbol = BEGIN;
	else if (strcmp(string, "END") == 0) t->symbol = END;
	else if (strcmp(string, "IF") == 0) t->symbol = IF;
	else if (strcmp(string, "ELSE") == 0) t->symbol = ELSE;
	else if (strcmp(string, "WHILE") == 0) t->symbol = WHILE;
	else if (strcmp(string, "ODD") == 0) t->symbol = ODD;
	else if (strcmp(string, "PRINT") == 0) t->symbol = PRINT;
	else if (strcmp(string, "SCAN") == 0) t->symbol = SCAN;
}