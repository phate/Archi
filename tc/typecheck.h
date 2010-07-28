#ifndef ARCHI_TC_TYPECHECK_H_
#define ARCHI_TC_TYPECHECK_H_

#include "../symtab.h"

#include <stdbool.h>

bool archi_id_def_check( archi_symtab *st, const char *id, archi_ast_node *n ) ;

archi_symtab_idlist* archi_variabledef_typecheck( archi_symtab *st, archi_ast_node *n, const char *id, archi_symtab_idlist *list ) ;

void archi_typecheck( archi_symtab *st, archi_ast_node *n ) ;

#endif
