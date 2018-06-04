/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#include <stdio.h>
#include <string.h>
#include "token.h"


void clear_token(token_t *t) {
	//printf("%d\n", sizeof(t->value));
	memset(t->value, 0, sizeof(t->value));
	t->num_value = 0;
	t->symbol = CONST;
}


void print_symbol(token_symbol_t s)
{
	switch (s) {
		case ASSIGN:
		printf("ASSIGN");
		break;
		
		case CONST:
		printf("CONST");
		break;

		case VAR:
		printf("VAR");
		break;

		case PROCEDURE:
		printf("PROCEDURE");
		break;

		case CALL:
		printf("CALL");
		break;

		case BEGIN:
		printf("BEGIN");
		break;

		case END:
		printf("END");
		break;

		case IF:
		printf("IF");
		break;

		case ELSE:
		printf("ELSE");
		break;
		
		case WHILE:
		printf("WHILE");
		break;

		case ODD:
		printf("ODD");
		break;

		case IDENT:
		printf("IDENT");
		break;

		case NUM:
		printf("NUM");
		break;

		case PLUS:
		printf("PLUS");
		break;

		case MINUS:
		printf("MINUS");
		break;

		case TIMES:
		printf("TIMES");
		break;

		case SLASH:
		printf("SLASH");
		break;

		case GTE:
		printf("GTE");
		break;

		case LTE:
		printf("LTE");
		break;

		case GREATER:
		printf("GREATER");
		break;

		case LESSER:
		printf("LESSER");
		break;

		case EQUAL:
		printf("EQUAL");
		break;

		case NOTEQUAL:
		printf("NOTEQUAL");
		break;

		case COMMA:
		printf("COMMA");
		break;

		case COLON:
		printf("COLON");
		break;

		case SEMICOLON:
		printf("SEMICOLON");
		break;

		case LPAREN:
		printf("LPAREN");
		break;

		case RPAREN:
		printf("RPAREN");
		break;

		case PRINT:
		printf("PRINT");
		break;

		case SCAN:
		printf("SCAN");
		break;

		case LIST_END:
		printf("LIST_END");
		break;

		default:
		printf("ERROR");
		break;
	}
}


void print_token(token_t t)
{
	printf("%s\n%d\n", t.value, t.num_value);
	print_symbol(t.symbol);
	printf("\n\n");
}