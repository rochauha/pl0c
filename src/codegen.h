/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm-c/Core.h>
#include "ast.h"
#include "symtab.h"


LLVMValueRef generate_code(ast_node_t* root, symbol_t* *symbol_table, size_t* current_level,
							LLVMModuleRef module, LLVMBuilderRef ir_builder);

#endif