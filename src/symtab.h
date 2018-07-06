/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>
#include <stdint.h>
#include "ast.h"


typedef enum {
	SYM_CONST,
	SYM_VAR,
	SYM_PROCEDURE,
} sym_type_t;


typedef struct symbol {
	char name[32];
	int64_t value;
	size_t level; // nesting level
	sym_type_t type;
	struct symbol *next;
	struct symbol *prev;
} symbol_t;


void run_semantic_checks(ast_node_t *root, symbol_t **symbol_table);

symbol_t* lookup(char *name);

void print_table(symbol_t **table);

bool semantic_error();

size_t symbol_count();

#endif