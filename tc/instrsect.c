#include "instrsect.h"
#include "../debug.h"

#include <string.h>

static void archi_expression_typecheck( archi_ast_node *n, const char* dtype ) ;

static const char* archi_expression_infer( archi_ast_node *n )
{
  DEBUG_ASSERT( n ) ;

  switch( n->node_type ){
    case NT_BSTR:
      return n->data_type ;
    case NT_CONCAT:{
      archi_expression_typecheck( n->first_child, "Bits" ) ;
      archi_expression_typecheck( n->last_child, "Bits" ) ;
      return "Bits" ;}
    default: DEBUG_ASSERT(0) ; break ; 
  }
}

static void archi_expression_typecheck( archi_ast_node *n, const char* dtype )
{
  DEBUG_ASSERT( n && dtype ) ;
 
  const char* type = archi_expression_infer( n ) ;
  if( strcmp(type, dtype) )
    EMSG_TYPE_MISMATCH( n, type, dtype ) ; 
}

static int32_t archi_instrdef_encoding_length( archi_ast_node *n )
{
  DEBUG_ASSERT( n ) ;

  switch( n->node_type ){
    case NT_BSTR:
      return n->attr.nt_bstr.len ;
    case NT_CONCAT:{
      n->attr.nt_concat.len = archi_instrdef_encoding_length( n->first_child ) ;
      n->attr.nt_concat.len += archi_instrdef_encoding_length( n->last_child ) ;
      return n->attr.nt_concat.len ;}
    default: DEBUG_ASSERT(0) ; return -1 ;
  }
}

static void archi_instrdef_encoding_typecheck( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_ENCODING ) ;

  archi_expression_typecheck( n->first_child, "Bits" ) ; 

  int32_t len = archi_instrdef_encoding_length( n->first_child ) ;
  if( len % 8 != 0 )
    EMSG_ENCODING_LENGTH( n ) ;    
}

static void archi_instrdef_io_tid_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_TID ) ;

  archi_ast_node *l = archi_symtab_lookup( st, n->data_type ) ;
  if( l == NULL ){
    EMSG_MISSING_REGISTER_CLASS( n, n->data_type ) ;
  }
  else if( strcmp(l->data_type, "RegClass") )
    EMSG_WRONG_TYPE( n, n->attr.nt_tid.id, "RegClass" ) ;

  l = archi_symtab_lookup( st, n->attr.nt_tid.id ) ;
  if( l != NULL ){
    EMSG_REDECLARATION( n, n->attr.nt_tid.id ) ;
    EMSG_PREVIOUS_DECLARATION( n, n->attr.nt_tid.id, l->linenr ) ;   
  }
  else archi_symtab_insert( st, n->attr.nt_tid.id, n ) ;
}

static void archi_instrdef_output_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_OUTPUT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    archi_instrdef_io_tid_typecheck( st, c ) ;
  }
}

static void archi_instrdef_input_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_INPUT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( !strcmp( c->data_type, "Int" ) ){
      archi_ast_node *l = archi_symtab_lookup( st, c->attr.nt_tid.id ) ;
      if( l != NULL ){
        EMSG_REDECLARATION( c, c->attr.nt_tid.id ) ;
        EMSG_PREVIOUS_DECLARATION( c, c->attr.nt_tid.id, l->linenr ) ;
      }
      else archi_symtab_insert( st, c->attr.nt_tid.id, c ) ;
    }
    else{
      archi_instrdef_io_tid_typecheck( st, c ) ;
    } 
  }
}

static void archi_instrdef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_INSTRDEF ) ;

  if( n->attr.nt_instrdef.input == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "input" ) ;
  else archi_instrdef_input_typecheck( st, n->attr.nt_instrdef.input ) ;

  if( n->attr.nt_instrdef.output == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "output" ) ;
  else archi_instrdef_output_typecheck( st, n->attr.nt_instrdef.output ) ;

  if( n->attr.nt_instrdef.encoding == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "encoding" ) ;
  else archi_instrdef_encoding_typecheck( n->attr.nt_instrdef.encoding ) ;
}

void archi_instrsect_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_INSTRSECT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_INSTRDEF ){
      archi_symtab_push_scope( st ) ;
      archi_instrdef_typecheck( st, c ) ;
      archi_symtab_pop_scope( st ) ;
    }
    else DEBUG_ASSERT(0) ;
  }
}
