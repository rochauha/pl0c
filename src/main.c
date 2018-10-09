/*
 * Copyright (c) Ronak Chauhan
 * This file is part of pl0c and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

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

    char* file_name = NULL;

    switch (argc) {
        case 1:
            fprintf(stderr, "error: no input file\n");
            exit(EXIT_FAILURE);

        case 2:
            file_name = argv[1];
            break;

        default:
            fprintf(stderr, "Usage: %s <file_name>.pl0\n", argv[0]);
            exit(EXIT_FAILURE);
    }

    char* token_buf;
    if (!scan(file_name, &token_buf)) {
        fprintf(stderr, "error: %s not found\n", file_name);
        exit(EXIT_FAILURE);
    }

    /* Memory buffer to be associated with the token stream */
    token_t* token_list = (token_t*)(token_buf);
    /*
    token_t *token_ptr = token_list;
    while (token_list->symbol != LIST_END) {
            print_token(*(token_list));
            token_list++;
    }*/

    set_token_ptr(&token_list);
    ast_node_t* root = parse();
    // printf("%zu\n", ast_node_count());
    // print_ast(root);

    symbol_t* symbol_table = NULL;
    size_t current_level = 0;
    run_semantic_checks(root, &symbol_table, &current_level);

    // print_table(&symbol_table);

    // printf("%zu\n", symbol_count());
    if (semantic_error())
        exit(EXIT_FAILURE);

    free(token_buf);

    /* No semantic error, so translate to LLVM IR */

    LLVMModuleRef module = LLVMModuleCreateWithName(file_name);
    LLVMBuilderRef builder = LLVMCreateBuilder();

    generate_code(root, &symbol_table, &current_level, module, builder);
    assert(current_level == 0);

    char* error_msg = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error_msg);

    // LLVMDumpModule(module);

    cleanup_ast(&root);
    assert(root == NULL);
    assert(ast_node_count() == 0);

    size_t len = strlen(file_name);
    file_name[len - 3] = 'l';
    file_name[len - 2] = 'l';
    file_name[len - 1] = '\0';

    LLVMPrintModuleToFile(module, file_name, &error_msg);
}