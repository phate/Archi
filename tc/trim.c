#include "trim.h"
#include "../ehandling.h"
#include "../debug.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


static bool archi_attribute_node_assign( archi_ast_node *emsgnode, archi_ast_node **target, archi_ast_node *attr, void* defval, const char* attrname )
{
  bool r = false ;
  if( *target == defval ){ *target = attr ; r = true ;}
  else EMSG_MULTIPLE_ATTRIBUTE( emsgnode, attrname ) ;

  return r ;
}

static bool archi_attribute_int_assign( archi_ast_node *emsgnode, int32_t *target, int32_t attr, int32_t defval, const char* attrname )
{
  bool r = false ;
  if( *target == defval ){ *target = attr ; r = true ;}
  else EMSG_MULTIPLE_ATTRIBUTE( emsgnode, attrname ) ;

  return r ;
}

static void archi_regdef_trim( archi_ast_node* n )
{
  DEBUG_ASSERT( n && n->node_type == NT_REGDEF ) ;

  bool free_node ;
	archi_ast_node *c = n->first_child ;
	while( c != NULL ){
		free_node = false ;
    switch( c->node_type ){
      case NT_CODE:{
        archi_attribute_int_assign( n, &n->attr.nt_regdef.code, c->attr.nt_code.code, -1, "code" ) ;
        free_node = true ;
        break ;}
      case NT_ID:{
        n->attr.nt_regdef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
        free_node = true ;    
        break ;}
      default: DEBUG_ASSERT(0) ;
    }

    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ; 
    c = nc ;
  }
}

static void archi_regcldef_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_REGCLDEF ) ;

  bool free_node ;
	archi_ast_node *c = n->first_child ;
	while( c != NULL ){
    free_node = false ;
    switch( c->node_type ){
    case NT_BITS:{
      archi_attribute_int_assign( n, &n->attr.nt_regcldef.bits, c->attr.nt_bits.bits, -1, "bits" ) ;
      free_node = true ;
      break ;}
    case NT_REGS:{
      bool r = archi_attribute_node_assign( n, &n->attr.nt_regcldef.regs, c, NULL, "regs" ) ;
      if( r ){
			  int32_t nregs = 0 ;
			  archi_ast_node *cc ;
			  FOREACH_CHILD( n->attr.nt_regcldef.regs, cc ) nregs++ ;
			  c->attr.nt_regs.nregs = nregs ;
      }
      break ;}
    case NT_ID:{
      n->attr.nt_regcldef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
      free_node = true ;
      break ;}
    default: DEBUG_ASSERT(0) ;
    }
    
    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ; 
    c = nc ;
	}
}

static void archi_regsect_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_REGSECT ) ;

  int32_t nregcls = 0 ;
	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		if( c->node_type == NT_REGDEF ) archi_regdef_trim( c ) ;
		else if( c->node_type == NT_REGCLDEF ){ archi_regcldef_trim( c ) ; nregcls++ ; }
		else DEBUG_ASSERT(0) ;
	}

  n->attr.nt_regsect.nregcls = nregcls ;
}


static void archi_bslc_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_BSLC ) ;

  if( n->first_child->next_sibling->node_type != NT_NUM )
    EMSG_COMPILE_TIME_CONSTANT( n->first_child->next_sibling ) ;
  else{
    n->attr.nt_bslc.start = n->first_child->next_sibling->attr.nt_num.num ;
    TALLOC_FREE( n->first_child->next_sibling ) ;
  }
  
  if( n->last_child->node_type != NT_NUM )
    EMSG_COMPILE_TIME_CONSTANT( n->last_child ) ;
  else{
    n->attr.nt_bslc.length = n->last_child->attr.nt_num.num ;
    TALLOC_FREE( n->last_child ) ;
  }
}

static void archi_ifthenelse_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_IFTHENELSE ) ;

  n->attr.nt_ifthenelse.pred = n->first_child ;
  n->attr.nt_ifthenelse.cthen = n->first_child->next_sibling ;
  n->attr.nt_ifthenelse.celse = n->last_child ;
}

static uint32_t archi_instrdef_encoding_trim( archi_ast_node *n )
{
  archi_ast_node *c ;
  uint32_t nifthenelse = 0 ;
  FOREACH_CHILD( n, c )
    nifthenelse += archi_instrdef_encoding_trim( c ) ;  

  if( n->node_type == NT_BSLC ) archi_bslc_trim( n ) ;  
  if( n->node_type == NT_IFTHENELSE ){
    nifthenelse++ ;
    archi_ifthenelse_trim( n ) ;
  }

  return nifthenelse ;
}

static void archi_instrdef_flags_trim( archi_ast_node *n )
{
  if( !n ) return ;
  DEBUG_ASSERT( n->node_type == NT_FLAGS ) ;
        
  #define X( nt, op, p ) \
    case nt : n->attr.nt_flags.flags |= p ; break ;

  archi_ast_node *c = n->first_child ;
  while( c != NULL ){
    switch( c->node_type ){
      ARCHI_INSTR_FLAGS
      default : DEBUG_ASSERT(0) ;
    }
    archi_ast_node *ns = c->next_sibling ;
    TALLOC_FREE( c ) ;
    c = ns ; 
  }

  #undef X
}

static void archi_instrdef_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRDEF ) ;

  uint32_t free_node ;
  archi_ast_node *c = n->first_child ;
	while( c != NULL ){
    free_node = 0 ;
    switch( c->node_type ){
    case NT_INPUT:{
        bool r = archi_attribute_node_assign( n, &n->attr.nt_instrdef.input, c, NULL, "input" ) ;
        if( r ){
          archi_ast_node *cc ;
          int32_t nints = 0, nregs = 0 ;
          FOREACH_CHILD( c, cc ){ !strcmp(cc->data_type, "Int") ? nints++ : nregs++  ;}
          n->attr.nt_instrdef.input->attr.nt_input.nints = nints ;
          n->attr.nt_instrdef.input->attr.nt_input.nregs = nregs ;
        }    
      break ;}
    case NT_OUTPUT:{
        bool r = archi_attribute_node_assign( n, &n->attr.nt_instrdef.output, c, NULL, "input" ) ;
        if( r ){
          int32_t nregs = 0 ;
          archi_ast_node *cc ;
          FOREACH_CHILD( c, cc ){ nregs++ ;}
          n->attr.nt_instrdef.output->attr.nt_output.nregs = nregs ;
        }
      break ;}
    case NT_ENCODING:{
      archi_attribute_node_assign( n, &n->attr.nt_instrdef.encoding, c, NULL, "encoding" ) ;
      break ;}
    case NT_FLAGS:{
      archi_attribute_node_assign( n, &n->attr.nt_instrdef.flags, c, NULL, "flags" ) ;
      break ;}
    case NT_ID:{
        n->attr.nt_instrdef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
        free_node = true ;
      break ;}
    default: DEBUG_ASSERT(0) ;
    }

    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ; 
    c = nc ;
  }

  uint32_t nifthenelse = archi_instrdef_encoding_trim( n->attr.nt_instrdef.encoding ) ;
  n->attr.nt_instrdef.encoding->attr.nt_encoding.nifthenelse = nifthenelse ; 
  archi_instrdef_flags_trim( n->attr.nt_instrdef.flags ) ;
}

static void archi_instrsect_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_INSTRSECT ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		if( c->node_type == NT_INSTRDEF ) archi_instrdef_trim( c ) ; 
		else DEBUG_ASSERT(0) ;
	}
}

static void archi_matchdef_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_MATCHDEF ) ;

  bool free_node ;
  archi_ast_node *c = n->first_child ;
  while( c != NULL ){
    free_node = false ;
    switch( c->node_type ){
      case NT_INPUT:{
        bool r = archi_attribute_node_assign( n, &n->attr.nt_matchdef.input, c, NULL, "input" ) ;
        if( r ){
          archi_ast_node *cc ;
          int32_t nints = 0, nregs = 0 ;
          FOREACH_CHILD( c, cc ){ !strcmp(cc->data_type, "Int") ? nints++ : nregs++  ;}
          n->attr.nt_matchdef.input->attr.nt_input.nints = nints ;
          n->attr.nt_matchdef.input->attr.nt_input.nregs = nregs ;
        }    
        break ;}
      case NT_OUTPUT:{
        bool r = archi_attribute_node_assign( n, &n->attr.nt_matchdef.output, c, NULL, "output" ) ;
        if( r ){
          int32_t nregs = 0 ;
          archi_ast_node *cc ;
          FOREACH_CHILD( c, cc ){ nregs++ ;}
          n->attr.nt_matchdef.output->attr.nt_output.nregs = nregs ;
        }
        break ;}
      case NT_IPATTERN:{
        archi_attribute_node_assign( n, &n->attr.nt_matchdef.ipattern, c, NULL, "ipattern" ) ;
        break ;}
      case NT_OPATTERN:{
        archi_attribute_node_assign( n, &n->attr.nt_matchdef.opattern, c, NULL, "opattern" ) ;
        break ;}
      case NT_ID:{
        n->attr.nt_matchdef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
        free_node = true ;
        break ;}
      default: DEBUG_ASSERT(0) ;
    }
  
    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ;
    c = nc ;
  }
}

static void archi_patternsect_trim( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_PATTERNSECT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD(n, c){
    if( c->node_type == NT_MATCHDEF ) archi_matchdef_trim( c ) ;
    else DEBUG_ASSERT(0) ;
  }
}

void archi_ast_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		switch( c->node_type ){
			case NT_REGSECT:
        n->attr.nt_archdef.regsect = c ;
        archi_regsect_trim( c ) ; break ;
			case NT_INSTRSECT:
        n->attr.nt_archdef.instrsect = c ;
        archi_instrsect_trim( c ) ; break ;
      case NT_PATTERNSECT:
        n->attr.nt_archdef.patternsect = c ;
        archi_patternsect_trim( c ) ; break ;
//			case AUXSECT:		archi_auxsect_trim( c ) ; break ;
			default: DEBUG_ASSERT(0) ;
		}	
	}
}
