/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE.md for more details.
 */


#ifndef LEXER_H
#define LEXER_H


#include "token.h"

size_t scan(const char *source, char **buf);

void set_keyword(token_t *t);

#endif