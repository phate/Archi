#include "prepare.h"
#include "../debug.h"

static void archi_nt_dot_prepare( archi_ast_node *n )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    archi_nt_dot_prepare( c ) ;
  } 

  if( n->node_type == NT_DOT ){
    archi_ast_node_data_type_set( n->first_child, n->data_type ) ;
    archi_ast_node_dreplace( n, n->first_child ) ; 
  }
}

static void archi_instrdef_prepare( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRDEF ) ; 

  archi_nt_dot_prepare( n->attr.nt_instrdef.encoding->first_child ) ; 
}

static void archi_instrsect_prepare( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRSECT ) ; 

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_INSTRDEF )
      archi_instrdef_prepare( c ) ; 
  }
}

void archi_code_generate_prepare( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ; 

  archi_instrsect_prepare( n->attr.nt_archdef.instrsect ) ; 

}
