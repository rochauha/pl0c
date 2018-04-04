/* 
 * Copyright (c) Ronak Chauhan
 * This file is part of plz and is licensed under the terms of the MIT License.
 * See LICENSE for more details.
 */


#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "lexer.h"
#include "parser.h"


int main(int argc, char **argv)
{	
	/* Memory buffer to be associated with the token stream */
	char *token_buf;
	scan(argv[1], &token_buf);

	token_t *token_list = (token_t *)(token_buf);
	
	//token_t *token_ptr = token_list;
	/*while (token_list->symbol != LIST_END) {
		print_token(*(token_list));
		token_list++;
	}*/

	set_token_ptr(&token_list);
	parse();

	free(token_buf);
}