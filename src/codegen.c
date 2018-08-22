/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

/*
 * Since semantic checks are already performed before calling anything defined here, 
 * no need to check return values for functions defined in symtab.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <llvm-c/Core.h>

#include "ast.h"
#include "symtab.h"
#include "codegen.h"


static LLVMValueRef number_value(ast_node_t* node)
{
	return LLVMConstInt(LLVMInt64Type(), node->num_value, true); 	
}


LLVMValueRef generate_code(ast_node_t* root, symbol_t** symbol_table, size_t* current_level,
							LLVMModuleRef module, LLVMBuilderRef ir_builder)
{
	if (root->label == AST_ROOT) {
		ast_node_t* current = root->first_child;
		while (current) {
			if (current->label == AST_CONST_DECL) {
				ast_node_t* c_ident = current->first_child;
				
				while (c_ident) {
					LLVMValueRef global_c = LLVMAddGlobal(module, LLVMInt64Type(), c_ident->ident_name);
					LLVMValueRef global_c_val = number_value(c_ident->first_child);
					LLVMSetInitializer (global_c, global_c_val);
				
					symbol_t* new_symtab_entry = new_symbol(c_ident->ident_name, SYM_CONST, 
							global_c_val, *current_level);
					insert_sym(symbol_table, new_symtab_entry);
					c_ident = c_ident->next_sibling;
				}
			}

			else if (current->label == AST_VAR_DECL) {
				ast_node_t* c_ident = current->first_child;
				ast_node_t* dummy_node = calloc(1, sizeof(ast_node_t));

				while (c_ident) {
					LLVMValueRef global_v = LLVMAddGlobal(module, LLVMInt64Type(), c_ident->ident_name);
					LLVMValueRef global_v_val = number_value(dummy_node);
					LLVMSetInitializer(global_v, global_v_val);

					symbol_t* new_symtab_entry = new_symbol(c_ident->ident_name, SYM_CONST,
					 					global_v_val, *current_level);
					insert_sym(symbol_table, new_symtab_entry);
					
					c_ident = c_ident->next_sibling;
				}
				free(dummy_node);
			}

			else if (current->label == AST_PROC_DECL) {
				
			}

			else if (current->label == AST_STMT_BLOCK) {
				/* Generate IR for main function here */

				LLVMTypeRef param_type_list = NULL;

				/* function signature corresponding to int main() which returns 0 at the end */
				LLVMTypeRef main_function_type = LLVMFunctionType(LLVMInt32Type(), param_type_list, 0, false);
				LLVMValueRef main = LLVMAddFunction(module, "main", main_function_type);
				
				LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
				LLVMPositionBuilderAtEnd(ir_builder, entry);

				ast_node_t* statement = current->first_child;
				while (statement) {
					// pending work
					statement = statement->next_sibling;
				}

				/* finally main() returns 0 */
				LLVMBuildRet(ir_builder, LLVMConstInt(LLVMInt32Type(), 0, true));
			}
			current = current->next_sibling;
		}

		free_current_scope(current_level);
		(*symbol_table) = NULL;
	}

	else if (root->label == AST_CONST_DECL) {
		ast_node_t* current = root->first_child;
		while (current) {
			symbol_t* new_symtab_entry = new_symbol(current->ident_name, 
						SYM_CONST, (LLVMValueRef)(current->first_child->num_value), *current_level);
			
			insert_sym(symbol_table, new_symtab_entry);
			current = current->next_sibling;
		}
	}

	else if (root->label == AST_VAR_DECL) {
		ast_node_t* current = root->first_child;
		while (current) {
			symbol_t* new_symtab_entry = new_symbol(current->ident_name, 
						SYM_VAR, (LLVMValueRef)(0), *current_level);

			insert_sym(symbol_table, new_symtab_entry);
			current = current->next_sibling;
		}
	}

	else if (root->label == AST_PROC_DECL) {
		ast_node_t* current = root->first_child;
		symbol_t* new_symtab_entry = new_symbol(root->ident_name,
						SYM_PROCEDURE, (LLVMValueRef)(0), *current_level);	
		
		(*current_level)++;
		current = current->next_sibling;
		generate_code(current, symbol_table, current_level, module, ir_builder);
		printf("%zu\n", free_current_scope(current_level));
		(*current_level)--;
	}

	else if (root->label == AST_STMT_BLOCK) {
		/* Go through all statments in this block */
		ast_node_t* current = root->first_child;

	}

	else {
		ast_node_t* c_root = root->first_child;
		while (c_root) {
			generate_code(c_root, symbol_table, current_level, module, ir_builder);
			c_root = c_root->next_sibling;	
		}
	}
}