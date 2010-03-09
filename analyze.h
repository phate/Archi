#ifndef ANALYZE_H_
#define ANALYZE_H_

#include "symtab.h"

#include <string.h>

#define MANGLE_NAME( name, prefix, postfix ) \
  char name[strlen(prefix)+strlen( strcmp(prefix, "") ? "::" : "")+strlen(postfix)+1] ; \
  sprintf( name, "%s%s%s", prefix, strcmp(prefix, "") ? "::" : "", postfix )

void fill_symtab( symtab stab, node *n ) ;

void typecheck( symtab stab, node *n ) ;

#endif
