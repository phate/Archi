#ifndef ARCHI_CGEN_H_
#define ARCHI_CGEN_H_

#include "../ast/node.h"

#include <stdio.h>

void archi_code_generate( archi_ast_node *n, FILE *hf, FILE *sf ) ;

#endif
