#ifndef ARCHI_TC_TYPECHECK_H_
#define ARCHI_TC_TYPECHECK_H_

#include "../symtab.h"

void archi_tidlist_typecheck( archi_symtab *st, archi_ast_node *n, archi_symtab_idlist *list ) ;

void archi_typecheck( archi_symtab *st, archi_ast_node *n ) ;

#endif
