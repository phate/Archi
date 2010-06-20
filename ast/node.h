#ifndef ARCHI_NODE_H_
#define ARCHI_NODE_H_

#include "../ehandling.h"
#include "attributes.h"

#include <talloc.h>

#include <stdint.h>

#define FOREACH_CHILD( p, c ) for( c = p->first_child; c != NULL; c = c->next_sibling )
#define FOREACH_SIBLING( p, c ) for( c = p; c != NULL; c = c->next_sibling ) 

#define SET_FIRST_CHILD( p, c ) do{ p->first_child = c ; c->parent = p ; }while(0)
#define SET_LAST_CHILD( p, c ) do{ p->last_child = c ; c->parent = p ; }while(0)
#define ARE_SIBLINGS( ls, rs ) do{ls->next_sibling = rs ; rs->prev_sibling = ls ;}while(0)

typedef enum{	NT_ARCHDEF,
							NT_REGSECT,
							INSTRSECT,					
							AUXSECT,
	
							NT_REGDEF,
							NT_CODE,
	
							NT_REGCLDEF,
							NT_REGS,
							NT_BITS,
							
							INSTRDEF,
							INPUT,
							OUTPUT,
							IMMEDIATE,
							ENCODING,

							FCTDEF,
							ARGS,
							IFTHENELSE,
							LOGICALOR,
							LOGICALAND,
							NOTEQUAL,
							EQUAL,
							LESSTHAN,
							GREATERTHAN,
							LESSTHANEQUAL,
							GREATERTHANEQUAL,
							SHIFTRIGHT,
							SHIFTLEFT,
							PLUS,
							MINUS,
							TIMES,
							DIVIDE,
							MOD,
							CONCATENATION,
							BITSLICE,
							PROPSELECTION,
							FCTCALL,
							NUMBER,
							BITSTRING,
							BOOLEAN,

							NT_ID,
							TID
						 } archi_ast_nodetype ;

typedef struct archi_ast_node_{
	archi_ast_nodetype node_type ;
	char* data_type ;										
	struct archi_ast_node_* first_child ;
	struct archi_ast_node_* last_child ;
	struct archi_ast_node_* next_sibling ;
	struct archi_ast_node_* prev_sibling ;
	struct archi_ast_node_* parent ;
	node_attributes attr ;
  uint32_t linenr ;
	archi_emsg* emsg_list ;
} archi_ast_node ;

/*
typedef struct instrprop_{
	node *input ;
	node *output ;
	node *immediates ;
	node *encoding ;
	const char *name ;
} instrprop ;

typedef struct fctprop_{
	node *args ;
	node *body ;
	const char *name ;
} fctprop ;
*/

//instrprop* create_instrprop() ;
//fctprop* create_fctprop() ;

archi_ast_node* archi_ast_node_talloc( TALLOC_CTX *ctx ) ;
void archi_ast_node_init( archi_ast_node *n, archi_ast_nodetype ntype, const char *dtype, unsigned int linenr ) ;

void archi_view_ast( archi_ast_node *n ) ;

#endif
