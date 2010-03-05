#ifndef SYMTAB_H_
#define SYMTAB_H_

#include "nodes.h"

#define SIZE 211

typedef struct entry_{
	struct entry_* next ;
	char* key ;
	node* n ;
} entry ;

typedef entry** symtab ;

symtab create_symtab() ;
void destroy_symtab( symtab t ) ;

node* lookup( symtab t, const char* key ) ;
void insert( symtab t, const char* key, node* n ) ;

void print_symtab( symtab t ) ;

#endif
