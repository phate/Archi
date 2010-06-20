#ifndef ARCHI_SYMTAB_H_
#define ARCHI_SYMTAB_H_

#include "ast/node.h"

/*
#define ARCHI_ENTRY_CNT 211

typedef struct archi_symtab_entry_{
	struct archi_symtab_entry_ *next ;
	char *key ;
	archi_ast_node *node ;
} archi_symtab_entry ;

typedef struct archi_symtab_scope_{
  struct archi_symtab_scope_* prev_scope ;
  archi_symtab_entry* entry[ARCHI_ENTRY_CNT] ;
} archi_symtab_scope ;
*/

struct archi_symtab_scope_ ;

typedef struct archi_symtab_{
  struct archi_symtab_scope_* innermost_scope ;
} archi_symtab ;

void archi_symtab_init( archi_symtab *st ) ;

archi_ast_node* archi_symtab_lookup( archi_symtab *st, const char* key ) ;
void archi_symtab_insert( archi_symtab *st, const char* key, archi_ast_node* n ) ;

void archi_symtab_push_scope( archi_symtab *st ) ;
void archi_symtab_pop_scope( archi_symtab *st ) ;

void archi_symtab_print( archi_symtab *st ) ;

#endif
