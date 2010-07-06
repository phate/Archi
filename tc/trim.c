#include "trim.h"
#include "../ehandling.h"
#include "../debug.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>


static void archi_regdef_trim( archi_ast_node* n )
{
  DEBUG_ASSERT( n && n->node_type == NT_REGDEF ) ;

  uint32_t free_node ;
	archi_ast_node *c = n->first_child ;
	while( c != NULL ){
		free_node = 0 ;
    if( c->node_type == NT_CODE ){
      if( n->attr.nt_regdef.code == -1 ){
        n->attr.nt_regdef.code = c->attr.nt_code.code ;
        free_node = 1 ; 
      }
      else EMSG_MULTIPLE_ATTRIBUTE( n, "code" ) ;
		}
		else if( c->node_type == NT_ID ){
			n->attr.nt_regdef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
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
			if( n->attr.nt_regcldef.bits == -1 ){
        n->attr.nt_regcldef.bits = c->attr.nt_bits.bits ;
        free_node = 1 ;
      }
			else EMSG_MULTIPLE_ATTRIBUTE( n, "bits" ) ;
		}
		else if( c->node_type == NT_REGS ){
			if( n->attr.nt_regcldef.regs == NULL ){
        n->attr.nt_regcldef.regs = c ;
			  int32_t i = 0 ;
			  archi_ast_node *cc ;
			  FOREACH_CHILD( n->attr.nt_regcldef.regs, cc ) i++ ;
			  c->attr.nt_regs.nregs = i ;
      }
		  else EMSG_MULTIPLE_ATTRIBUTE( n, "regs" ) ;
    }
		else if( c->node_type == NT_ID ){
			n->attr.nt_regcldef.id = talloc_strdup( n, c->attr.nt_id.id ) ;
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

  n->attr.nt_regsect.nregcls = nregcls ;
}

static void archi_instrdef_trim( archi_ast_node *n )
{
	DEBUG_ASSERT( n && n->node_type == NT_INSTRDEF ) ;

  uint32_t free_node ;
  archi_ast_node *c = n->first_child ;
	while( c != NULL ){
    free_node = 0 ;
		if( c->node_type == NT_INPUT ){
			if( n->attr.nt_instrdef.input == NULL ){
        int32_t nints = 0 ;
        int32_t nregs = 0 ;
        archi_ast_node *cc ;
        FOREACH_CHILD( c, cc ){ !strcmp(cc->data_type, "Int") ? nints++ : nregs++  ;}
        c->attr.nt_input.nints = nints ;
        c->attr.nt_input.nregs = nregs ;
        n->attr.nt_instrdef.input = c ;
      }
      else EMSG_MULTIPLE_ATTRIBUTE( n, "input" ) ;
    }
		else if( c->node_type == NT_OUTPUT ){
			if( n->attr.nt_instrdef.output == NULL ){
        int32_t i = 0 ;
        archi_ast_node *cc ;
        FOREACH_CHILD( c, cc ){ i++ ;}
        c->attr.nt_output.nregs = i ;
        n->attr.nt_instrdef.output = c ;
      }
      else EMSG_MULTIPLE_ATTRIBUTE( n, "output" ) ;
    }
		else if( c->node_type == NT_ENCODING ){
			if( n->attr.nt_instrdef.encoding == NULL ){
        n->attr.nt_instrdef.encoding = c ;
      }
      else EMSG_MULTIPLE_ATTRIBUTE( n, "encoding" ) ;
		}
    else if( c->node_type == NT_ID ){
		  n->attr.nt_instrdef.id = talloc_strdup( n, c->attr.nt_id.id ) ;	
	    free_node = 1 ;
    }

    archi_ast_node *nc = c->next_sibling ;
    if( free_node ) TALLOC_FREE( c ) ; 
    c = nc ;
  }
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
/*
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
			case NT_REGSECT:    archi_regsect_trim( c ) ; break ;
			case NT_INSTRSECT:  archi_instrsect_trim( c ) ; break ;
//			case AUXSECT:		archi_auxsect_trim( c ) ; break ;
			default: DEBUG_ASSERT(0) ;
		}	
	}
}
