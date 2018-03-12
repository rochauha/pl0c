/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE.md for more details.
 */


#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "lexer.h"


int main(int argc, char **argv)
{	
	/* Memory buffer to be associated with the token stream */
	char *token_buf;
	size_t token_list_len = scan(argv[1], &token_buf);

	token_t *token_list = (token_t *)(token_buf);
	printf("%zu\n", token_list_len);

	for (int i = 0; i < token_list_len; i++) {
		print_token(token_list[i]);
	}

	free(token_buf);
}