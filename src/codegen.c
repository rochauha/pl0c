/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

/*
 * Since semantic checks are already performed before calling anything defined
 * here, no need to check return values for functions defined in symtab.c
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "codegen.h"
#include "symtab.h"

static LLVMValueRef number_value(ast_node_t* node)
{
    return LLVMConstInt(LLVMInt64Type(), node->num_value, true);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wreturn-type"

static LLVMValueRef expression(ast_node_t* node, LLVMBuilderRef ir_builder)
{
    ast_node_t* lhs_node = NULL;
    ast_node_t* rhs_node = NULL;

    LLVMValueRef variable_location_on_stack = NULL;

    switch (node->label) {
        case AST_ADD:
            lhs_node = node->first_child;
            rhs_node = lhs_node->next_sibling;
            return LLVMBuildAdd(ir_builder, expression(lhs_node, ir_builder),
                                expression(rhs_node, ir_builder), "");

        case AST_SUB:
            lhs_node = node->first_child;
            rhs_node = lhs_node->next_sibling;
            return LLVMBuildSub(ir_builder, expression(lhs_node, ir_builder),
                                expression(rhs_node, ir_builder), "");

        case AST_MUL:
            lhs_node = node->first_child;
            rhs_node = lhs_node->next_sibling;
            return LLVMBuildMul(ir_builder, expression(lhs_node, ir_builder),
                                expression(rhs_node, ir_builder), "");

        case AST_DIV:
            lhs_node = node->first_child;
            rhs_node = lhs_node->next_sibling;
            return LLVMBuildSDiv(ir_builder, expression(lhs_node, ir_builder),
                                 expression(rhs_node, ir_builder), "");

        case AST_NUM:
            return number_value(node);

        case AST_IDENT:
            /* Load from memory location based on previous alloca/store
             * instruction */
            variable_location_on_stack = lookup(node->ident_name)->value;
            return LLVMBuildLoad(ir_builder, variable_location_on_stack, "");
    }
}

#pragma clang diagnostic pop

static void assignment(ast_node_t* node, LLVMBuilderRef ir_builder)
{
    /* After evaluating the expression, store the answer's value in symbol
     * table. This same location is used by load instructions when the
     * identifier comes up in an expression.
     */
    ast_node_t* lhs_node = node->first_child;
    ast_node_t* rhs_node = lhs_node->next_sibling;

    LLVMValueRef variable_location_on_stack = lookup(lhs_node->ident_name)->value;

    LLVMBuildStore(ir_builder, expression(rhs_node, ir_builder),
                   variable_location_on_stack);
}

void generate_globals(ast_node_t* current, symbol_t** symbol_table,
                      size_t* current_level, LLVMModuleRef module,
                      LLVMBuilderRef ir_builder)
{
    if (current->label == AST_CONST_DECL) {
        ast_node_t* c_ident = current->first_child;
        while (c_ident) {
            LLVMValueRef global_c =
                LLVMAddGlobal(module, LLVMInt64Type(), c_ident->ident_name);

            LLVMValueRef global_c_val = number_value(c_ident->first_child);

            LLVMSetInitializer(global_c, global_c_val);

            symbol_t* new_symtab_entry =
                new_symbol(c_ident->ident_name, SYM_CONST,
                           LLVMGetNamedGlobal(module, c_ident->ident_name),
                           *current_level);

            insert_sym(symbol_table, new_symtab_entry);

            c_ident = c_ident->next_sibling;
        }
    }

    else if (current->label == AST_VAR_DECL) {
        ast_node_t* c_ident = current->first_child;
        ast_node_t* dummy_node = calloc(1, sizeof(ast_node_t));

        while (c_ident) {
            LLVMValueRef global_v =
                LLVMAddGlobal(module, LLVMInt64Type(), c_ident->ident_name);
            LLVMValueRef global_v_val = number_value(dummy_node);
            LLVMSetInitializer(global_v, global_v_val);

            symbol_t* new_symtab_entry =
                new_symbol(c_ident->ident_name, SYM_CONST,
                           LLVMGetNamedGlobal(module, c_ident->ident_name),
                           *current_level);
            insert_sym(symbol_table, new_symtab_entry);

            c_ident = c_ident->next_sibling;
        }
        free(dummy_node);
    }
}

static void generate_statement(ast_node_t* node, LLVMModuleRef module,
                               LLVMBuilderRef ir_builder)
{
    switch (node->label) {
        case AST_ASSIGN:
            assignment(node, ir_builder);
            return;
        case AST_CALL:
            return;
        case AST_PRINT:
            return;
        case AST_IF:
            return;
    }
}

static void generate_function(ast_node_t* node, LLVMModuleRef module,
                              LLVMBuilderRef ir_builder)
{
    ast_node_t* function_head = node->first_child;
    ast_node_t* function_body = function_head->next_sibling; // AST_BLOCK

    ast_node_t* current = function_body->first_child;

    LLVMTypeRef* param_type_list = NULL;

    LLVMTypeRef function_type =
        LLVMFunctionType(LLVMVoidType(), param_type_list, 0, false);
    LLVMValueRef function =
        LLVMAddFunction(module, function_head->ident_name, function_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(function, "entry");
    LLVMPositionBuilderAtEnd(ir_builder, entry);
    LLVMBuildRetVoid(ir_builder);
}

LLVMValueRef generate_code(ast_node_t* root, symbol_t** symbol_table,
                           size_t* current_level, LLVMModuleRef module,
                           LLVMBuilderRef ir_builder)
{
    if (root->label == AST_ROOT) {
        ast_node_t* current = root->first_child;
        while (current) {
            if (current->label == AST_CONST_DECL || current->label == AST_VAR_DECL) {
                generate_globals(current, symbol_table, current_level, module,
                                 ir_builder);
            }

            else if (current->label == AST_PROC_DECL) {
                (*current_level)++;
                generate_function(current, module, ir_builder);
                (*current_level)--;
            }

            else if (current->label == AST_STMT_BLOCK) {
                /* Generate IR for main function here */

                LLVMTypeRef* param_type_list = NULL;

                /* function signature corresponding to int main() which
                 * returns 0 at the end */
                LLVMTypeRef main_function_type =
                    LLVMFunctionType(LLVMInt32Type(), param_type_list, 0, false);
                LLVMValueRef main =
                    LLVMAddFunction(module, "main", main_function_type);

                LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
                LLVMPositionBuilderAtEnd(ir_builder, entry);

                ast_node_t* statement = current->first_child;

                while (statement) {
                    generate_statement(statement, module, ir_builder);
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
}