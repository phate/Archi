#include "trim.h"
#include "../ehandling.h"
#include "../debug.h"

#include <stdlib.h>
#include <assert.h>


static void archi_regdef_trim( archi_ast_node* n )
{
  DEBUG_ASSERT( n && n->node_type == NT_REGDEF ) ;

  uint32_t free_node ;
	archi_ast_node *c = n->first_child ;
	while( c != NULL ){
		free_node = 0 ;
    if( c->node_type == NT_CODE ){
      if( n->attr.reg->code == -1 ){
        n->attr.reg->code = c->attr.code ;
        free_node = 1 ; 
      }
      else EMSG_MULTIPLE_ATTRIBUTE( n, "code" ) ;
		}
		else if( c->node_type == NT_ID ){
			n->attr.reg->id = talloc_strdup( n->attr.reg, c->attr.id ) ;
      free_node = 1 ;
    }

    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ; 
    c = nc ;
  }
}

static void archi_regcldef_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_REGCLDEF ) ;

  uint32_t free_node ;
	archi_ast_node *c = n->first_child ;
	while( c != NULL ){
    free_node = 0 ;
		if( c->node_type == NT_BITS ){
			if( n->attr.regcl->bits == -1 ){
        n->attr.regcl->bits = c->attr.bits ;
        free_node = 1 ;
      }
			else EMSG_MULTIPLE_ATTRIBUTE( n, "bits" ) ;
		}
		else if( c->node_type == NT_REGS ){
			if( n->attr.regcl->regs == NULL ){
        n->attr.regcl->regs = c ;
			  int32_t i = 0 ;
			  archi_ast_node *cc ;
			  FOREACH_CHILD( n->attr.regcl->regs, cc ) i++ ;
			  c->attr.nregs = i ;
      }
		  else EMSG_MULTIPLE_ATTRIBUTE( n, "regs" ) ;
    }
		else if( c->node_type == NT_ID ){
			n->attr.regcl->id = talloc_strdup( n->attr.regcl, c->attr.id ) ;
      free_node = 1 ;
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

  n->attr.nregcls = nregcls ;
}
/*
static void archi_instrdef_trim( archi_ast_node *n )
{
	assert(0) ;
  assert( n->node_type == INSTRDEF ) ;
*/
/*
	archi_ast_node *c ;
	instrprop *p = (instrprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == INPUT ){
			if( p->input == NULL ) p->input = c ;
      else EMSG_MISSING_PROPERTY( n, "input" ) ;
    }
		else if( c->ntype == OUTPUT ){
			if( p->output == NULL ) p->output = c ;
      else EMSG_MISSING_PROPERTY( n, "output" ) ;
    }
		else if( c->ntype == IMMEDIATE ){
			if( p->immediates == NULL ) p->immediates = c ;
      else EMSG_MISSING_PROPERTY( n, "immediates" ) ;
    }
		else if( c->ntype == ENCODING )
			if( p->encoding == NULL ) p->encoding = c ;
      else EMSG_MISSING_PROPERTY( n, "encoding" ) ;
		else if( c->ntype == ID )
			p->name = (const char*)c->data ;
	}

}
*/
/*
static void archi_instrsect_trim( archi_ast_node *n )
{
	assert( n->node_type == INSTRSECT ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		if( c->node_type == INSTRDEF ) archi_instrdef_trim( c ) ; 
		else assert(0) ;
	}
}

static void archi_fctdef_trim( archi_ast_node *n )
{
  assert(0) ;
	assert( n->node_type == FCTDEF ) ;

	archi_ast_node *c ;
	fctprop *p = (fctprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == ARGS )
			p->args = c->first_child ;
		else if( c->ntype == TID )
			p->name = (const char*)c->data ;
		else p->body = c ;
	}

}
*/
/*
static void archi_auxsect_trim( archi_ast_node *n )
{
	assert( n->node_type == AUXSECT ) ;

	archi_ast_node *c ;
	FOREACH_CHILD( n, c ){
		if( c->node_type == FCTDEF ) archi_fctdef_trim( c ) ;
		else assert(0) ;
	}
}
*/
void archi_ast_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		switch( c->node_type ){
			case NT_REGSECT: 	archi_regsect_trim( c ) ; break ;
//			case INSTRSECT:	archi_instrsect_trim( c ) ; break ;
//			case AUXSECT:		archi_auxsect_trim( c ) ; break ;
			default: DEBUG_ASSERT(0) ;
		}	
	}
}
