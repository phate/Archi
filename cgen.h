#ifndef CGEN_H_
#define CGEN_H_

#include "ast/node.h"

#include <stdio.h>

void generate_code( archi_ast_node *n, FILE *hf, FILE *sf ) ;

#endif
