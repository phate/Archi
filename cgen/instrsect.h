#ifndef ARCHI_CGEN_INSTRSECT_H_
#define ARCHI_CGEN_INSTRSECT_H_

#include "../ast/node.h"
#include "../symtab.h"

void archi_instrsect_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf ) ;

#endif
