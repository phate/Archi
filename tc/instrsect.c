#include "instrsect.h"
#include "typecheck.h"
#include "../debug.h"

#include <string.h>
#include <math.h>

static char* archi_expression_typecheck( archi_symtab *st, archi_ast_node *n, char* dtype ) ;
static char* archi_binop_typecheck( archi_symtab *st, archi_ast_node *n, char* dtypes[], uint32_t cnt ) ;

static char* archi_nt_dot_attribute_typecheck( archi_ast_node *n, const char* ps_type )
{
  if( n->node_type == NT_ID ){
    if( !strcmp(ps_type, "Int") ){
      if( !strcmp(n->attr.nt_id.id, "bstr") ) return "Bits" ;
    }
    if( !strcmp(ps_type, "Bits") ){
      if( !strcmp(n->attr.nt_id.id, "value") ) return "Int" ;
    }
    if( !strcmp(ps_type, "RegClass") ){
      if( !strcmp(n->attr.nt_id.id, "code") ) return "Int" ;
      if( !strcmp(n->attr.nt_id.id, "reg") ) return "Reg" ;
    } 
  }  

  if( n->node_type == NT_DOT ){
    const char* type = archi_nt_dot_attribute_typecheck( n->first_child, ps_type ) ;
    return archi_nt_dot_attribute_typecheck( n->last_child, type ) ;
  }

  if( n->node_type == NT_ID )
    EMSG_MISSING_ATTRIBUTE( n, n->attr.nt_id.id ) ;
  else
    EMSG_NOT_AN_ATTRIBUTE( n ) ; 

  //FIXME: write message that it defaults to int
  return "Int" ;
}

static char* archi_expression_infer( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( n ) ;

  switch( n->node_type ){
    case NT_ID:{
      archi_ast_node *l = archi_symtab_lookup( st, n->attr.nt_id.id ) ;
      if( l == NULL ){
        EMSG_MISSING_ID( n, n->attr.nt_id.id ) ;
        return "Int" ;  //FIXME: write message that it defaults to Int
      }
      archi_ast_node_data_type_set( n, l->data_type ) ;
      return l->data_type ;}
    case NT_NUM:
    case NT_STR:
    case NT_BSTR:
    case NT_TRUE:
    case NT_FALSE:
      return n->data_type ;
    case NT_IFTHENELSE:{
      archi_expression_typecheck( st, n->attr.nt_ifthenelse.pred, "Bool" ) ;
      char *type = archi_expression_infer( st, n->attr.nt_ifthenelse.cthen ) ;
      archi_expression_typecheck( st, n->attr.nt_ifthenelse.celse, type ) ;
      archi_ast_node_data_type_set( n, type ) ;
      return type ;}
    case NT_EQUAL:{
      archi_binop_typecheck( st, n, (char* [4]){"Int", "Bits", "Bool", "Reg"}, 4 ) ;
      //FIXME: check whether register occurs in register class
      archi_ast_node_data_type_set( n, "Bool" ) ;
      return "Bool" ;}
    case NT_CONCAT:{
      archi_expression_typecheck( st, n->first_child, "Bits" ) ;
      archi_expression_typecheck( st, n->last_child, "Bits" ) ;
      archi_ast_node_data_type_set( n, "Bits" ) ;
      return "Bits" ;}
    case NT_BSLC:{
      archi_expression_typecheck( st, n->first_child, "Bits" ) ;
      archi_ast_node_data_type_set( n, "Bits" ) ;
      return "Bits" ;}
    case NT_DOT:{
      char* type = archi_expression_infer( st, n->first_child ) ;
      if( !strcmp(type, "RegClass") || !strcmp(type, "Reg") ) EMSG_TYPE_NOT_SUPPORTED(n->first_child, type) ;
      
      archi_ast_node *l = archi_symtab_lookup( st, type ) ;
      if( l != NULL ) if( !strcmp( l->data_type, "RegClass") ) type = "RegClass" ;
      type = archi_nt_dot_attribute_typecheck( n->last_child, type ) ;
      archi_ast_node_data_type_set( n, type ) ;
      return type ;}
    default: DEBUG_ASSERT(0) ; break ; 
  }
}

static char* archi_binop_typecheck( archi_symtab *st, archi_ast_node *n, char* dtypes[], uint32_t cnt )
{
  DEBUG_ASSERT( n && dtypes ) ;

  uint32_t i ;
  const char* type = archi_expression_infer( st, n->first_child ) ;
  for( i = 0; i < cnt; i++ ){
    if( !strcmp(type, dtypes[i]) ) break ;
  }

  if( i == cnt ){
    EMSG_TYPE_NOT_SUPPORTED( n, n->first_child->data_type ) ;
    //FIXME: print error message for returning default value
    return "Int" ;
  }

  return archi_expression_typecheck( st, n->last_child, dtypes[i] ) ; 
}

static char* archi_expression_typecheck( archi_symtab *st, archi_ast_node *n, char* dtype )
{
  DEBUG_ASSERT( n && dtype ) ;
 
  const char* type = archi_expression_infer( st, n ) ;
  if( strcmp(type, dtype) )
    EMSG_TYPE_MISMATCH( n, type, dtype ) ;

  return dtype ;
}
/*
static uint32_t archi_paths_encoding_length( archi_ast_node *n, int32_t lengths[], uint32_t cnt, uint32_t tcnt )
{
  DEBUG_ASSERT( n ) ;

  int32_t length = 0 ;
  switch( n->node_type ){
    case NT_BSTR:
      length = n->attr.nt_bstr.len ; break ;
    case NT_BSLC:
      length = n->attr.nt_bslc.length ; break ;
    case NT_DOT:
      length = -1 ; break ;
    case NT_CONCAT:{
      cnt = archi_paths_encoding_length( n->first_child, lengths, cnt, tcnt ) ;
      cnt = archi_paths_encoding_length( n->last_child, lengths, cnt, tcnt ) ;
      break ;}
    case NT_IFTHENELSE:{
      memcpy( lengths+(tcnt/2), lengths, cnt*sizeof(int32_t) ) ;
      return2_t ncnt = archi_paths_encoding_length( n->attr.nt_ifthenelse.cthen, lengths, cnt, tcnt/2 ) ; 
      ncnt += archi_paths_encoding_length( n->attr.nt_ifthenelse.celse, lengths+cnt, cnt, tcnt/2 ) ;
      return ncnt ; break ;}
    default: DEBUG_ASSERT(0) ;
  }

  for( uint32_t i = 0; i < cnt; i++ ){
    if( length != -1 ) lengths[i] += length ;
    else lengths[i] = length ;
  }

  return cnt ;
}

static void archi_instrdef_encoding_length( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_ENCODING ) ;
  
  uint32_t npaths = pow(2, n->attr.nt_encoding.nifthenelse) ;
  int32_t lengths[npaths] ;
  memset( lengths, 0, npaths*sizeof(int32_t) ) ;
  
  archi_paths_encoding_length( n->first_child, lengths, 1, npaths ) ;

  printf( "Encoding lengths (cnt:%d):\n", npaths ) ;
  for( uint32_t i = 0; i < npaths; i++ ){
    printf( "%d\n", lengths[i] ) ;
    if( lengths[i] == -1 || lengths[i] % 8 != 0 ){
      EMSG_ENCODING_LENGTH( n ) ;
      //break ;
    }
  }
}
*/

//FIXME: implement the checking for the encoding length again
//FIXME: check whether bitslices indeces are compile time constants( do that in trim)
//FIXME: check whether the start+length of the bitslice is smaller than the bitstring it gets in
static void archi_instrdef_encoding_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_ENCODING ) ;

  archi_expression_typecheck( st, n->first_child, "Bits" ) ; 
  //archi_instrdef_encoding_length( n ) ;
}

static void archi_instrdef_io_tid_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_TID ) ;

  archi_ast_node *l = archi_symtab_lookup( st, n->data_type ) ;
  if( l == NULL ){
    EMSG_MISSING_REGISTER_CLASS( n, n->data_type ) ;
    return ;
  }

  if( strcmp(l->data_type, "RegClass") ){
    EMSG_WRONG_TYPE( n, n->attr.nt_tid.id, "RegClass" ) ;
    return ;
  }

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

//FIXME: not correct anymore, an id could also be used in a comparison for example
static int32_t archi_instrdef_io_usage_typecheck( const char* id, archi_ast_node *n )
{
  DEBUG_ASSERT( id && n ) ;

  if( n->node_type == NT_ID && !strcmp(n->attr.nt_id.id,id) ) return 1 ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    int32_t r = archi_instrdef_io_usage_typecheck( id, c ) ;
    if( r == 1 ) return r ;
  }

  return 0 ;
}

static void archi_instrdef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_INSTRDEF ) ;

  if( n->attr.nt_instrdef.input != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_create( NULL ) ;
    l = archi_symtab_idlist_fill( l, st, NT_REGCLDEF ) ; 
    l = archi_symtab_idlist_add( l, (char*[1]){"Int"}, 1 ) ; 
    archi_tidlist_typecheck( st, n->attr.nt_instrdef.input->first_child, l ) ; 
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "input" ) ;
  

  if( n->attr.nt_instrdef.output == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "output" ) ;
  else archi_instrdef_output_typecheck( st, n->attr.nt_instrdef.output ) ;

  if( n->attr.nt_instrdef.encoding == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "encoding" ) ;
  else archi_instrdef_encoding_typecheck( st, n->attr.nt_instrdef.encoding ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n->attr.nt_instrdef.input, c ){
    if( !archi_instrdef_io_usage_typecheck( c->attr.nt_tid.id, n->attr.nt_instrdef.encoding ) )
      EMSG_ID_NOT_USED( c, c->attr.nt_tid.id ) ;
  }

/* this check for output is not correct (e.g. flag register)
  FOREACH_CHILD( n->attr.nt_instrdef.output, c ){
    if( !archi_instrdef_io_usage_typecheck( c->attr.nt_tid.id, n->attr.nt_instrdef.encoding ) )
      EMSG_ID_NOT_USED( c, c->attr.nt_tid.id ) ;
  }
*/      
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
    else if( c->node_type == NT_JVINSTRDEF ) ;
    else DEBUG_ASSERT(0) ;
  }
}
