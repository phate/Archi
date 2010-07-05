#ifndef ARCHI_NODE_H_
#define ARCHI_NODE_H_

#include "../ehandling.h"
#include "attributes.h"
#include "node_types.h"

#include <talloc.h>

#include <stdint.h>

#define FOREACH_CHILD( p, c ) for( c = p->first_child; c != NULL; c = c->next_sibling )
#define FOREACH_NEXT_SIBLING( n, s ) for( s = n; s != NULL; s = s->next_sibling ) 
#define FOREACH_PREV_SIBLING( n, s ) for( s = n; s != NULL; s = s->prev_sibling )

//#define SET_FIRST_CHILD( p, c ) do{ p->first_child = c ; c->parent = p ; }while(0)
//#define SET_LAST_CHILD( p, c ) do{ p->last_child = c ; c->parent = p ; }while(0)
//#define ARE_SIBLINGS( ls, rs ) do{ls->next_sibling = rs ; rs->prev_sibling = ls ;}while(0)

#define X(a) a,
typedef enum{ ARCHI_AST_NODETYPE } archi_ast_nodetype ;
#undef X

/*
typedef enum{	NT_ARCHDEF,
							NT_REGSECT,
							NT_INSTRSECT,					
							AUXSECT,
	
							NT_REGDEF,
							NT_CODE,
	
							NT_REGCLDEF,
							NT_REGS,
							NT_BITS,
							
							NT_INSTRDEF,
							NT_INPUT,
							NT_OUTPUT,
							//IMMEDIATE,
							//ENCODING,

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
							NT_TID
						 } archi_ast_nodetype ;
*/

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


/** \brief Allocates a node properly.
*
* \param ctx  The talloc context to create the node on
*             or NULL to create the node as new top level context.
* \return     A properly allocated node.
*/
archi_ast_node* archi_ast_node_talloc( TALLOC_CTX *ctx ) ;


/** \brief Initializes a node.
*
* \param n      The node which is to be initialized.
* \param ntype  The type of the node.
* \param dtype  The data type of the node.
* \param linenr The line number of the node.  
*/
void archi_ast_node_init( archi_ast_node *n, archi_ast_nodetype ntype, const char *dtype, unsigned int linenr ) ;


/** \brief Allocates and initializes a node.
*
* \param ctx    The talloc context to create the node on
*               or NULL to create the node as new top level context. 
* \param ntype  The type of the node.
* \param dtype  The data type of the node.
* \param linenr The line number of node.
*
* It is equivalent to:
* @code
*   archi_ast_node *n = archi_ast_node_talloc( ctx ) ;
*   archi_ast_node_init( n, ntype, dtype, linenr ) ;
* @endcode
*
* \sa archi_ast_node_talloc() and archi_ast_node_init()
*/
archi_ast_node* archi_ast_node_create( TALLOC_CTX *ctx, archi_ast_nodetype ntype, const char *dtype, unsigned int linenr ) ;


/** \brief Disconnects a node from its parent and sibling nodes.
*
* \param n The node to be disconnected.
*/
void archi_ast_node_disconnect( archi_ast_node *n ) ;


/** \brief Sets the first child of a node.
*
* \param p  The parent node.
* \param fc The child node. 
*/
void archi_ast_node_first_child_set( archi_ast_node *p, archi_ast_node *fc ) ;


/** \brief  Disconnects a node from its parent and siblings and sets it
*           as first child of another node.
*
* \param p  The node the first child is set for.
* \param fc The node that is disconnected and set as a first child.
*
* It is equivalent to:
* @code
*   archi_ast_node_disconnect( fc ) ;
*   archi_ast_node_first_child_set( p, fc ) ;
* @endcode
*
* \sa archi_ast_node_disconnect() and archi_ast_node_first_child_set()
*/
void archi_ast_node_first_child_dset( archi_ast_node *p, archi_ast_node *fc ) ;


/** \brief Sets the last child of a node.
*
* \param p  The parent node. 
* \param lc The child node.
*/
void archi_ast_node_last_child_set( archi_ast_node *p, archi_ast_node *lc ) ;


/** \brief  Disconnects a node from its parent and siblings and sets it
*           as last child of another node.
*
* \param p  The node the last child is set for.
* \param fc The node that is disconnected and set as a last child.
*
* It is equivalent to:
* @code
*   archi_ast_node_disconnect( fc ) ;
*   archi_ast_node_last_child_set( p, fc ) ;
* @endcode
*
* \sa archi_ast_node_disconnect() and archi_ast_node_last_child_set()
*/
void archi_ast_node_last_child_dset( archi_ast_node *p, archi_ast_node *lc ) ; 


/** \brief Sets the next sibling of a node.
*
* \param n  The node the next sibling is set for.
* \param ns The next sibling node.
*/
void archi_ast_node_next_sibling_set( archi_ast_node *n, archi_ast_node *ns ) ;


/** \brief  Disconnects a node from its parent and siblings and sets it
*           as next sibling of another node.
*
* \param p  The node the next sibling is set for.
* \param fc The node that is disconnected and set as a next sibling.
*
* It is equivalent to:
* @code
*   archi_ast_node_disconnect( fc ) ;
*   archi_ast_node_next_sibling_set( p, fc ) ;
* @endcode
*
* \sa archi_ast_node_disconnect() and archi_ast_node_next_sibling_set()
*/
void archi_ast_node_next_sibling_dset( archi_ast_node *n, archi_ast_node *ns ) ;


/** \brief Sets the previous sibling of a node.
*
* \param n  The node the previous sibling is set for.
* \param ps The previous sibling node. 
*/
void archi_ast_node_prev_sibling_set( archi_ast_node *n, archi_ast_node *ps ) ;


/** \brief  Disconnects a node from its parent and siblings and sets it
*           as previous sibling of another node.
*
* \param p  The node the previous sibling is set for.
* \param fc The node that is disconnected and set as a previous sibling.
*
* It is equivalent to:
* @code
*   archi_ast_node_disconnect( fc ) ;
*   archi_ast_node_prev_sibling_set( p, fc ) ;
* @endcode
*
* \sa archi_ast_node_disconnect() and archi_ast_node_prev_sibling_set()
*/
void archi_ast_node_prev_sibling_dset( archi_ast_node *n, archi_ast_node *ps ) ;


void archi_view_ast( archi_ast_node *n ) ;

#endif
