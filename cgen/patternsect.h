#ifndef ARCHI_CGEN_PATTERNSECT_H_
#define ARCHI_CGEN_PATTERNSECT_H_

#include "../ast/node.h"
#include "../symtab.h"

void archi_patternsect_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf ) ;

#endif
