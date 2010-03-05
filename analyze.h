#ifndef ANALYZE_H_
#define ANALYZE_H_

#include "symtab.h"

void fill_symtab( symtab stab, node *n ) ;

void typecheck( symtab stab, node *n ) ;

#endif
