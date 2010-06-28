#ifndef ARCHI_CGEN_H_
#define ARCHI_CGEN_H_

#include "../ast/node.h"
#include "../symtab.h"

#include <stdio.h>

void archi_code_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf ) ;

#endif
