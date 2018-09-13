/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>

#include "ast.h"
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "symtab.h"
#include "token.h"

int main(int argc, char** argv)
{
    /* Memory buffer to be associated with the token stream */
    char* token_buf;
    scan(argv[1], &token_buf);

    token_t* token_list = (token_t*)(token_buf);
    /*
    token_t *token_ptr = token_list;
    while (token_list->symbol != LIST_END) {
            print_token(*(token_list));
            token_list++;
    }*/

    set_token_ptr(&token_list);
    ast_node_t* root = parse();
    printf("%zu\n", ast_node_count());
    print_ast(root);

    symbol_t* symbol_table = NULL;
    size_t current_level = 0;
    run_semantic_checks(root, &symbol_table, &current_level);

    // print_table(&symbol_table);

    // printf("%zu\n", symbol_count());
    if (semantic_error())
        exit(EXIT_FAILURE);

    free(token_buf);

    /*
    The tree is not deleted manually (intentionally) as of now because rest
    of the things are not in place yet.
    */

    /*
    No semantic error, so translate to LLVM IR and spit out ELF binary
    */

    LLVMModuleRef module = LLVMModuleCreateWithName(argv[1]);
    LLVMBuilderRef builder = LLVMCreateBuilder();

    generate_code(root, &symbol_table, &current_level, module, builder);

    assert(current_level == 0);

    char* error_msg = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error_msg);
    LLVMDisposeMessage(error_msg);
    LLVMDumpModule(module);
}