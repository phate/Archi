#ifndef ARCHI_ANALYZE_H_
#define ARCHI_ANALYZE_H_

#include "../symtab.h"

//#include <string.h>

/* #define MANGLE_NAME( name, prefix, postfix ) \
//  char name[strlen(prefix)+strlen( strcmp(prefix, "") ? "::" : "")+strlen(postfix)+1] ; \
  sprintf( name, "%s%s%s", prefix, strcmp(prefix, "") ? "::" : "", postfix )
*/

//void archi_symtab_fill( archi_symtab stab, archi_ast_node *n ) ;

void archi_typecheck( archi_symtab *st, archi_ast_node *n ) ;

#endif
