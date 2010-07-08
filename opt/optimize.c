#include "optimize.h"
#include "../debug.h"

static void archi_nt_dot_optimize( archi_ast_node *n )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ) archi_nt_dot_optimize( c ) ;

  if( n->node_type == NT_DOT ){
    archi_ast_node_dreplace( n, n->first_child ) ;
  }
}

void archi_instrdef_optimize( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRDEF ) ;

  archi_nt_dot_optimize( n->attr.nt_instrdef.encoding->first_child ) ;
}

void archi_instrsect_optimize( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRSECT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_INSTRDEF )
      archi_instrdef_optimize( c ) ; 
  }
}

void archi_code_optimize( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_INSTRSECT )
      archi_instrsect_optimize( c ) ;
  }
}
