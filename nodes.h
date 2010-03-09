#ifndef NODE_H_
#define NODE_H_

#include "ehandling.h"

#include <stdint.h>

//change them
#define FOREACH_CHILD( p, c ) for( c = p->first_child; c != NULL; c = c->next_sibling )
//#define FOREACH_CHILD_R( p, c ) for( c = p; c != NULL; c = c->prev_sibling )
#define FOREACH_SIBLING( p, c ) for( c = p; c != NULL; c = c->next_sibling ) 

//#define FOREACH_SIBLING_R( p, c ) for( c = p; c->prev_sibling != NULL; c = c->prev_sibling ) 
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
						 } nodetype ;

typedef struct node_{
	nodetype ntype ;
	char* dtype ;										
	struct node_* first_child ;
	struct node_* last_child ;
	struct node_* next_sibling ;
	struct node_* prev_sibling ;
	struct node_* parent ;
	void* data ;
	uint32_t linenr ;
	emsg* emsgs ;
} node ;

typedef struct regprop_{
	int32_t code ;
	const char *name ;
} regprop ;

typedef struct regclprop_{
	int32_t bits ;
	node *regs ;
	const char *name ;
} regclprop ;

typedef struct instrprop_{
	node *input ;
	node *output ;
	node *immediates ;
	const char *name ;
} instrprop ;

typedef struct fctprop_{
	node *args ;
	node *body ;
	const char *name ;
} fctprop ;

regprop* create_regprop() ;
regclprop* create_regclprop() ;
instrprop* create_instrprop() ;
fctprop* create_fctprop() ;

node* create_node( nodetype ntype, char* dtype, void* data, unsigned int linenr ) ;
void destroy_node( node* p ) ;

void view_tree( node* n ) ;

#endif
