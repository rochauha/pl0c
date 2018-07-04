/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef LEXER_H
#define LEXER_H


#include "token.h"

void scan(const char *source, char **buf);

#endif