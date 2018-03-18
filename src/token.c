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


void print_token(token_t t)
{
	printf("%s\n%d\n", t.value, t.num_value);
	switch (t.symbol) {
		case CONST:
		printf("CONST\n");
		break;

		case VAR:
		printf("VAR\n");
		break;

		case PROCEDURE:
		printf("PROCEDURE\n");
		break;

		case CALL:
		printf("CALL\n");
		break;

		case BEGIN:
		printf("BEGIN\n");
		break;

		case END:
		printf("END\n");
		break;

		case IF:
		printf("IF\n");
		break;

		case THEN:
		printf("THEN\n");
		break;
		
		case WHILE:
		printf("WHILE\n");
		break;

		case DO:
		printf("DO\n");
		break;

		case ODD:
		printf("ODD\n");
		break;

		case IDENT:
		printf("IDENT\n");
		break;

		case NUM:
		printf("NUM\n");
		break;

		case PLUS:
		printf("PLUS\n");
		break;

		case MINUS:
		printf("MINUS\n");
		break;

		case TIMES:
		printf("TIMES\n");
		break;

		case SLASH:
		printf("SLASH\n");
		break;

		case GTE:
		printf("GTE\n");
		break;

		case LTE:
		printf("LTE\n");
		break;

		case GREATER:
		printf("GREATER\n");
		break;

		case LESSER:
		printf("LESSER\n");
		break;

		case EQUAL:
		printf("EQUAL\n");
		break;

		case NOTEQUAL:
		printf("NOTEQUAL\n");
		break;

		case COMMA:
		printf("COMMA\n");
		break;

		case SEMICOLON:
		printf("SEMICOLON\n");
		break;

		case QUESTIONMARK:
		printf("QUESTIONMARK\n");
		break;

		case EXCLAMATION:
		printf("EXCLAMATION\n");
		break;

		case LPAREN:
		printf("LPAREN\n");
		break;

		case RPAREN:
		printf("RPAREN\n");
		break;

		case LIST_END:
		printf("LIST_END");
		break;

		default:
		printf("ERROR\n");
		break;
	}
	printf("\n");
}