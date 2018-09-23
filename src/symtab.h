/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

typedef enum {
    SYM_CONST,
    SYM_VAR,
    SYM_PROCEDURE,
} sym_type_t;

typedef struct symbol {
    char name[32];
    LLVMValueRef value;
    size_t level; // nesting level
    sym_type_t type;
    struct symbol* next;
    struct symbol* prev;
} symbol_t;

void run_semantic_checks(ast_node_t* root, symbol_t** symbol_table,
                         size_t* current_level);

symbol_t* lookup(char* name);

symbol_t* new_symbol(char* name, sym_type_t type, LLVMValueRef value, size_t level);

size_t free_current_scope(size_t* current_level);

bool insert_sym(symbol_t** table, symbol_t* new_symbol_obj);

void print_table(symbol_t** table);

bool semantic_error();

size_t symbol_count();

void update_ident_value(char* ident_name, LLVMValueRef value);

void save_scope(symbol_t** location, size_t* current_level);

#endif