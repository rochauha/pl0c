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

const int MAX = 1000;
static int index = 0;
static symbol_t* scope_array[MAX];

static void cleanup_scopes()
{
    symbol_t* tmp = NULL;
    symbol_t* current = NULL;
    for (int i = 0; i < index; i++) {
        if (scope_array[i]) {
            current = scope_array[i];
            while (current) {
                tmp = current;
                current = current->next;
                free(tmp);
            }
        }
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wreturn-type"

static LLVMValueRef number_value(ast_node_t* node)
{
    return LLVMConstInt(LLVMInt64Type(), node->num_value, true);
}

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

static void generate_globals(ast_node_t* current, symbol_t** symbol_table,
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

static void generate_locals(ast_node_t* current, symbol_t** symbol_table,
                            size_t* current_level, LLVMModuleRef module,
                            LLVMBuilderRef ir_builder)
{
    if (current->label == AST_CONST_DECL) {
        ast_node_t* c_ident = current->first_child;
        while (c_ident) {
            LLVMValueRef local_c =
                LLVMBuildAlloca(ir_builder, LLVMInt64Type(), c_ident->ident_name);
            LLVMBuildStore(ir_builder, number_value(c_ident->first_child), local_c);
            symbol_t* new_symtab_entry =
                new_symbol(c_ident->ident_name, SYM_CONST, local_c, *current_level);

            insert_sym(symbol_table, new_symtab_entry);
            c_ident = c_ident->next_sibling;
        }
    }

    else if (current->label == AST_VAR_DECL) {
        ast_node_t* c_ident = current->first_child;
        ast_node_t* dummy_node = calloc(1, sizeof(ast_node_t));

        while (c_ident) {
            LLVMValueRef local_v =
                LLVMBuildAlloca(ir_builder, LLVMInt64Type(), c_ident->ident_name);
            LLVMBuildStore(ir_builder, number_value(dummy_node), local_v);
            symbol_t* new_symtab_entry =
                new_symbol(c_ident->ident_name, SYM_CONST, local_v, *current_level);

            insert_sym(symbol_table, new_symtab_entry);
            c_ident = c_ident->next_sibling;
        }
        free(dummy_node);
    }
}

static void generate_statement(ast_node_t* node, symbol_t** symbol_table,
                               LLVMModuleRef module, LLVMBuilderRef ir_builder,
                               LLVMValueRef function_ref)
{
    if (node->label == AST_ASSIGN) {
        assignment(node, ir_builder);
    }

    else if (node->label == AST_CALL) {
        symbol_t* function_sym = lookup(node->first_child->ident_name);
        assert(function_sym);
        LLVMBuildCall(ir_builder, function_sym->value, NULL, 0, "");
    }

    else if (node->label == AST_PRINT) {
        LLVMValueRef print64 = LLVMGetNamedFunction(module, "print64");

        ast_label_t label = node->first_child->label;
        LLVMValueRef num = NULL;
        if (label == AST_NUM) {
            num = number_value(node->first_child);
        }

        else if (label == AST_IDENT) {
            LLVMValueRef variable_location_on_stack =
                lookup(node->first_child->ident_name)->value;
            num = LLVMBuildLoad(ir_builder, variable_location_on_stack, "");
        }
        LLVMBuildCall(ir_builder, print64, &num, 1, "");
    }

    else if (node->label == AST_SCAN) {
        LLVMValueRef scan64 = LLVMGetNamedFunction(module, "scan64");
        LLVMValueRef num = LLVMBuildCall(ir_builder, scan64, NULL, 0, "");
        LLVMValueRef variable_location_on_stack =
            lookup(node->first_child->ident_name)->value;
        LLVMBuildStore(ir_builder, num, variable_location_on_stack);
    }

    else if (node->label == AST_WHILE || node->label == AST_IF) {
        LLVMIntPredicate cmp;
        LLVMBasicBlockRef condition_block =
            LLVMAppendBasicBlock(function_ref, "condition_block");
        LLVMBasicBlockRef then_block =
            LLVMAppendBasicBlock(function_ref, "then_block");
        LLVMBasicBlockRef else_block =
            LLVMAppendBasicBlock(function_ref, "else_block");
        LLVMBasicBlockRef end_block =
            LLVMAppendBasicBlock(function_ref, "end_block");
        LLVMBuildBr(ir_builder, condition_block);
        LLVMPositionBuilderAtEnd(ir_builder, condition_block);

        ast_node_t* child = node->first_child;
        switch (child->label) {
            case AST_GTE:
                cmp = LLVMIntSGE;
                break;
            case AST_LTE:
                cmp = LLVMIntSLE;
                break;
            case AST_GT:
                cmp = LLVMIntSGT;
                break;
            case AST_LT:
                cmp = LLVMIntSLT;
                break;
            case AST_EQ:
                cmp = LLVMIntEQ;
                break;
            case AST_NEQ:
                cmp = LLVMIntNE;
                break;
            case AST_ODD:
                cmp = LLVMIntNE;
                break;
        }
        LLVMValueRef lhs = expression(child->first_child, ir_builder);
        LLVMValueRef rhs = NULL;

        if (child->label == AST_ODD) {
            // handle ODD keyword separately -> expression % 2 != 0
            lhs = LLVMBuildSRem(ir_builder, lhs,
                                LLVMConstInt(LLVMInt64Type(), 2, true), "");
            rhs = LLVMConstInt(LLVMInt64Type(), 0, true);
        }

        else {
            rhs = expression(child->first_child->next_sibling, ir_builder);
        }

        LLVMValueRef condition =
            LLVMBuildICmp(ir_builder, cmp, lhs, rhs, "condition");

        LLVMBuildCondBr(ir_builder, condition, then_block, else_block);
        LLVMPositionBuilderAtEnd(ir_builder, then_block);

        child = child->next_sibling;
        if (child->label == AST_STMT_BLOCK) {
            ast_node_t* statement = child->first_child;
            while (statement) {
                generate_statement(statement, symbol_table, module, ir_builder,
                                   function_ref);
                statement = statement->next_sibling;
            }
        } else {
            generate_statement(child, symbol_table, module, ir_builder,
                               function_ref);
        }
        /* last part, jump to condition again if it is a while loop */
        if (node->label == AST_WHILE) {
            LLVMBuildBr(ir_builder, condition_block);
        } else {
            LLVMBuildBr(ir_builder, end_block);
        }

        LLVMPositionBuilderAtEnd(ir_builder, else_block);
        /* Code for the else block. When no else block exists or in case of a while
         * loop, simply branch to end_block */
        child = child->next_sibling;
        if (child && child->label == AST_STMT_BLOCK) {
            ast_node_t* statement = child->first_child;
            while (statement) {
                generate_statement(statement, symbol_table, module, ir_builder,
                                   function_ref);
                statement = statement->next_sibling;
            }
        } else if (child) {
            generate_statement(child, symbol_table, module, ir_builder,
                               function_ref);
        }
        LLVMBuildBr(ir_builder, end_block);
        LLVMPositionBuilderAtEnd(ir_builder, end_block);
    }
}

static void generate_function(ast_node_t* node, symbol_t** symbol_table,
                              size_t* current_level, LLVMModuleRef module,
                              LLVMBuilderRef ir_builder)
{
    ast_node_t* function_head = node->first_child;
    ast_node_t* function_body = function_head->next_sibling; // AST_BLOCK

    LLVMTypeRef* param_type_list = NULL;
    LLVMTypeRef function_type =
        LLVMFunctionType(LLVMVoidType(), param_type_list, 0, false);
    LLVMValueRef function =
        LLVMAddFunction(module, function_head->ident_name, function_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(function, "entry");
    LLVMPositionBuilderAtEnd(ir_builder, entry);

    /* Add this function's details to symbol table */
    (*current_level)--;
    symbol_t* new_symtab_entry = new_symbol(function_head->ident_name, SYM_PROCEDURE,
                                            function, *current_level);
    insert_sym(symbol_table, new_symtab_entry);
    (*current_level)++;

    ast_node_t* current = function_body->first_child;
    ast_node_t* statement = NULL;
    while (current) {
        if (current->label == AST_CONST_DECL || current->label == AST_VAR_DECL) {
            generate_locals(current, symbol_table, current_level, module,
                            ir_builder);
        } else if (current->label == AST_STMT_BLOCK) {
            statement = current->first_child;
            while (statement) {
                generate_statement(statement, symbol_table, module, ir_builder,
                                   function);
                statement = statement->next_sibling;
            }
        } else if (current) { // single statement
            generate_statement(current, symbol_table, module, ir_builder, function);
        }
        current = current->next_sibling;
    }
    LLVMBuildRetVoid(ir_builder);
}

static bool stmt_starts(ast_node_t* node)
{
    ast_label_t label = node->label;
    return label == AST_IF || label == AST_ASSIGN || label == AST_CALL ||
           label == AST_WHILE;
}

void generate_code(ast_node_t* root, symbol_t** symbol_table, size_t* current_level,
                   LLVMModuleRef module, LLVMBuilderRef ir_builder)
{
    LLVMTypeRef print64_param_type_list[] = { LLVMInt64Type() };
    LLVMTypeRef print64_type =
        LLVMFunctionType(LLVMVoidType(), print64_param_type_list, 1, false);

    LLVMTypeRef* scan64_param_type_list = NULL;
    LLVMTypeRef scan64_type =
        LLVMFunctionType(LLVMInt64Type(), scan64_param_type_list, 0, false);

/* Ignore unused variable warnings here */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    LLVMValueRef print64 = LLVMAddFunction(module, "print64", print64_type);
    LLVMValueRef scan64 = LLVMAddFunction(module, "scan64", scan64_type);
#pragma clang diagnostic pop

    if (root->label == AST_ROOT) {
        ast_node_t* current = root->first_child;
        while (current) {
            if (current->label == AST_CONST_DECL || current->label == AST_VAR_DECL) {
                generate_globals(current, symbol_table, current_level, module,
                                 ir_builder);
            }

            else if (current->label == AST_PROC_DECL) {
                (*current_level)++;
                generate_function(current, symbol_table, current_level, module,
                                  ir_builder);
                save_scope(&scope_array[index], current_level);
                index++;
                (*current_level)--;
            }

            else if (current->label == AST_STMT_BLOCK || stmt_starts(current)) {
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

                ast_node_t* statement =
                    current->first_child ? current->first_child : current;
                while (statement) {
                    generate_statement(statement, symbol_table, module, ir_builder,
                                       main);
                    statement = statement->next_sibling;
                }
                /* finally main() returns 0 */
                LLVMBuildRet(ir_builder, LLVMConstInt(LLVMInt32Type(), 0, true));
            }
            current = current->next_sibling;
        }
        save_scope(&scope_array[index], current_level);
        index++;
        cleanup_scopes();
    }
}

#pragma clang diagnostic pop