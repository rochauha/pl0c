/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

void set_token_ptr(token_t** t);

ast_node_t* parse();

#endif