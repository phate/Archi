#include "node.h"
#include "../debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*
instrprop* create_instrprop()
{
	instrprop *p = (instrprop*)malloc( sizeof(instrprop) ) ;

	p->input = NULL ;
	p->output = NULL ;
	p->immediates = NULL ;
	p->encoding = NULL ;
	p->name = NULL ;

	return p ;
}

fctprop* create_fctprop()
{
	fctprop *p = (fctprop*)malloc( sizeof(fctprop) ) ;

	p->args = NULL ;
	p->body = NULL ;
	p->name = NULL ;	

	return p ;
}
*/

/*
archi_ast_node* archi_ast_node_create( archi_ast_nodetype ntype, char* dtype, unsigned int linenr )
{
	archi_ast_node *n = malloc( sizeof(archi_ast_node) ) ;

	n->node_type = ntype ;
	n->data_type = dtype ;
	n->first_child = NULL ;
	n->last_child = NULL ;
	n->next_sibling = NULL ;
	n->prev_sibling = NULL ;
	n->parent = NULL ;
	n->linenr = linenr ;
	n->emsg_list = NULL ;

  memset( n->attr, 0, sizeof(node_attributes) ) ;

	return n ;
}
*/
static int archi_ast_node_destroy( archi_ast_node *n )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    talloc_free( c ) ;
  }

  if( n->prev_sibling ) n->prev_sibling->next_sibling = n->next_sibling ;
  else if( n->parent != NULL ) n->parent->first_child = n->next_sibling ;

  if( n->next_sibling ) n->next_sibling->prev_sibling = n->prev_sibling ;
  else if( n->parent != NULL ) n->parent->last_child = n->prev_sibling ;

  return 0 ;
}

archi_ast_node* archi_ast_node_talloc( TALLOC_CTX *ctx )
{
  archi_ast_node *n = talloc( ctx, archi_ast_node ) ;
  talloc_set_destructor( n, archi_ast_node_destroy ) ;

  return n ;
}

void archi_ast_node_init( archi_ast_node *n, archi_ast_nodetype ntype, const char* dtype, unsigned int linenr )
{
  DEBUG_ASSERT( n ) ;

  n->node_type = ntype ;
  n->data_type =  dtype == NULL ? NULL : talloc_strdup( n, dtype ) ;
  n->first_child = NULL ;
  n->last_child = NULL ;
  n->next_sibling = NULL ;
  n->prev_sibling = NULL ;
  n->parent = NULL ;
  n->linenr = linenr ;
  n->emsg_list = NULL ; 

  //FIXME: initialize union
}

archi_ast_node* archi_ast_node_create( TALLOC_CTX *ctx, archi_ast_nodetype ntype, const char* dtype, unsigned int linenr )
{
  archi_ast_node *n = archi_ast_node_talloc( ctx ) ;
  archi_ast_node_init( n, ntype, dtype, linenr ) ;

  return n ;
}

void archi_ast_node_disconnect( archi_ast_node *n )
{
  DEBUG_ASSERT( n ) ;

  if( n->prev_sibling ) n->prev_sibling->next_sibling = n->next_sibling ;
  if( n->next_sibling ) n->next_sibling->prev_sibling = n->prev_sibling ;

  if( n->parent != NULL ){
    if( n->parent->first_child == n ) n->parent->first_child = n->next_sibling ;
    if( n->parent->last_child == n ) n->parent->last_child = n->prev_sibling ;
  }

  n->parent = NULL ;
  n->prev_sibling = NULL ;
  n->next_sibling = NULL ;
}

void archi_ast_node_first_child_set( archi_ast_node *p, archi_ast_node *fc )
{
  DEBUG_ASSERT( p && fc ) ;
 
  if( p->first_child ){
    p->first_child->prev_sibling = fc ;
    fc->next_sibling = p->first_child ;
  }
  else p->last_child = fc ;

  fc->parent = p ;
  p->first_child = fc ;
}

void archi_ast_node_first_child_dset( archi_ast_node *p, archi_ast_node *fc )
{
  archi_ast_node_disconnect( fc ) ;
  archi_ast_node_first_child_set( p, fc ) ; 
}

void archi_ast_node_last_child_set( archi_ast_node *p, archi_ast_node *lc )
{
  DEBUG_ASSERT( p && lc ) ;
  
  if( p->last_child ){
      p->last_child->next_sibling = lc ;
      lc->prev_sibling = p->last_child ;
  }
  else p->first_child = lc ;

  lc->parent = p ;
  p->last_child = lc ;
}

void archi_ast_node_last_child_dset( archi_ast_node *p, archi_ast_node *lc )
{
  archi_ast_node_disconnect( lc ) ;
  archi_ast_node_last_child_set( p, lc ) ;
}

void archi_ast_node_next_sibling_set( archi_ast_node *n, archi_ast_node *ns )
{
  DEBUG_ASSERT( n && ns ) ;

  ns->prev_sibling = n ; 
  ns->parent = n->parent ;
  n->next_sibling = ns ;
}

void archi_ast_node_next_sibling_dset( archi_ast_node *n, archi_ast_node *ns )
{
  archi_ast_node_disconnect( ns ) ;
  archi_ast_node_next_sibling_set( n, ns ) ;
}

void archi_ast_node_prev_sibling_set( archi_ast_node *n, archi_ast_node *ps )
{
  DEBUG_ASSERT( n && ps ) ;

  ps->next_sibling = n ;
  ps->parent = n->parent ;
  n->prev_sibling = ps ;
}

void archi_ast_node_prev_sibling_dset( archi_ast_node *n, archi_ast_node *ps )
{
  archi_ast_node_disconnect( ps ) ;
  archi_ast_node_prev_sibling_set( n, ps ) ;
}

void archi_ast_node_replace( archi_ast_node *o, archi_ast_node *n )
{
  archi_ast_node *ns = o->next_sibling ;
  archi_ast_node *ps = o->prev_sibling ;
  archi_ast_node *p = o->parent ;
  
  TALLOC_FREE( o ) ;
  
  n->parent = p ;
  n->next_sibling = ns ;
  n->prev_sibling = ps ;
  
  if( ns != NULL ) ns->prev_sibling = n ;
  else p->last_child = n ;
 
  if( ps != NULL ) ps->next_sibling = n ;
  else p->first_child = n ;
}

void archi_ast_node_dreplace( archi_ast_node *o, archi_ast_node *n )
{
  archi_ast_node_disconnect( n ) ;
  archi_ast_node_replace( o, n ) ; 
}

void archi_ast_node_data_type_set( archi_ast_node *n, char* dtype )
{
  TALLOC_FREE( n->data_type ) ;
  n->data_type = talloc_strdup( n, dtype ) ;
}

#define X(a) #a,
static const char* nodetype_name[] = { ARCHI_AST_NODETYPE } ;
#undef X

//FIXME: fixed size array
static void node2string( FILE* f, archi_ast_node *n )
{
	char str[256] ;

	const char* name = nodetype_name[n->node_type] ;
	DEBUG_ASSERT( name ) ;
	
	switch( n->node_type ){
    case NT_ENCODING:
      sprintf( str, "nif: %d", n->attr.nt_encoding.nifthenelse ) ; break ;
		case NT_CODE:
			sprintf( str, "%d", n->attr.nt_code.code ) ; break ;
		case NT_REGS:
			sprintf( str, "%d", n->attr.nt_regs.nregs ) ; break ;
		case NT_BITS:
			sprintf( str, "%d", n->attr.nt_bits.bits ) ; break ;
		case NT_ID:
			sprintf( str, "%s %s", n->data_type, n->attr.nt_id.id ) ; break ;
    case NT_NODEDEF:
      sprintf( str, "type: %s", n->data_type ) ; break ;
    case NT_DOT:
      sprintf( str, "type: %s", n->data_type ) ; break ;
    case NT_FLAGS:
      sprintf( str, "%d", n->attr.nt_flags.flags ) ; break ;
 		case NT_TID:
			sprintf( str, "%s %s", n->data_type, n->attr.nt_tid.id ) ; break ;
/*		case NUMBER:
			sprintf( str, "%s", (const char*)n->data ) ; break ;
*/  case NT_BSTR:
			sprintf( str, "%s", n->attr.nt_bstr.bstr ) ; break ;
    case NT_STR:
      sprintf( str, "%s", n->attr.nt_str.str ) ; break ;
		default:
			sprintf( str, "%s", "" ) ; break ;

	}
	
	const char* string = "\"n: %p\\np: %p\\nfc: %p\\nlc: %p\\nns: %p\\nps: %p\\n%s\\n%s\\nlnr:%d\"" ;					
	fprintf( f, string, n, n->parent, n->first_child,
		n->last_child, n->next_sibling, n->prev_sibling, name, str, n->linenr ) ;
}

static void write_node( FILE *f, archi_ast_node *p )
{
	archi_ast_node *c ;
	FOREACH_CHILD( p, c ){
		node2string( f, p ) ;
		fprintf( f, " -> " ) ;
		node2string( f, c ) ;
		fprintf( f, " ;\n" ) ;
		write_node( f, c ) ;
	}
}

void archi_view_ast( archi_ast_node* n )
{
	if( !n ) return ;

	FILE* f = fopen( "/tmp/graph.dot", "w" ) ;
	if( !f ){ 
		fprintf( stderr, "Error: Cannot create file /tmp/graph.dot.\n" ) ;
		return ;
	}

	fprintf( f, "digraph G{\n") ;
	write_node( f, n ) ;
	fprintf( f, "}\n" ) ;
	fclose( f ) ;

	system( "rm -f /tmp/graph.ps && dot -Tps /tmp/graph.dot >> /tmp/graph.ps && evince /tmp/graph.ps " ) ;
}
