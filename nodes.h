#ifndef NODE_H_
#define NODE_H_

#include "ehandling.h"

#include <stdint.h>

//change them
#define FOREACH_CHILD( p, c ) for( c = p->first_child; c != NULL; c = c->next_sibling )
#define FOREACH_SIBLING( p, c ) for( c = p; c != NULL; c = c->next_sibling ) 

#define SET_FIRST_CHILD( p, c ) do{ p->first_child = c ; c->parent = p ; }while(0)
#define SET_LAST_CHILD( p, c ) do{ p->last_child = c ; c->parent = p ; }while(0)
#define ARE_SIBLINGS( ls, rs ) do{ls->next_sibling = rs ; rs->prev_sibling = ls ;}while(0)

typedef enum{	ARCHDEF,
							REGSECT,
							INSTRSECT,					
							AUXSECT,
	
							REGDEF,
							CODE,
	
							REGCLDEF,
							REGS,
							BITS,
							
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

							ID,
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
	void* data ;
	uint32_t linenr ;
	archi_emsg* emsg_list ;
} archi_ast_node ;

typedef struct archi_reg_attributes_{
	int32_t code ;
	const char *name ;
} archi_reg_attributes ;

typedef struct archi_regcl_attributes_{
	int32_t bits ;
	archi_ast_node *regs ;
	const char *name ;
} archi_regcl_attributes ;
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

void archi_reg_attributes_init( archi_reg_attributes *attr ) ;
void archi_regcl_attributes_init( archi_regcl_attributes *attr ) ;

//instrprop* create_instrprop() ;
//fctprop* create_fctprop() ;

archi_ast_node* archi_ast_node_create( archi_ast_nodetype ntype, char *dtype, void *data, unsigned int linenr ) ;
void archi_ast_node_destroy( archi_ast_node *p ) ;

void archi_view_ast( archi_ast_node *n ) ;

#endif
