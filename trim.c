#include "trim.h"
#include "ehandling.h"

#include <stdlib.h>
#include <assert.h>

static void archi_regdef_trim( archi_ast_node* n )
{
  assert( n->node_type == REGDEF ) ;

	archi_ast_node *c ;
	archi_reg_attributes *attr = (archi_reg_attributes*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->node_type == CODE ){
      if( attr->code == -1 ) attr->code = *((int32_t*)c->data) ;
      else EMSG_MULTIPLE_ATTRIBUTE( n, "code" ) ;
		}
		else if( c->node_type == ID )
			attr->name = (const char*)c->data ;
	}
}

static void archi_regcldef_trim( archi_ast_node *n )
{
	assert( n->node_type == REGCLDEF ) ;

	archi_ast_node *c ;
	archi_regcl_attributes *attr = (archi_regcl_attributes*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->node_type == BITS ){
			if( attr->bits == -1 ) attr->bits = *((int32_t*)c->data) ;
			else EMSG_MULTIPLE_ATTRIBUTE( n, "bits" ) ;
		}
		else if( c->node_type == REGS ){
			if( attr->regs == NULL ){
        attr->regs = c ;
			  int32_t i = 0 ;
			  archi_ast_node *cc ;
			  FOREACH_CHILD( attr->regs, cc ) i++ ;
			  *((int32_t*)c->data) = i ;
      }
		  else EMSG_MULTIPLE_ATTRIBUTE( n, "regs" ) ;
    }
		else if( c->node_type == ID )
			attr->name = (const char*)c->data ;
	}
}

static void archi_regsect_trim( archi_ast_node *n )
{
	assert( n->node_type == REGSECT ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		if( c->node_type == REGDEF ) archi_regdef_trim( c ) ;
		else if( c->node_type == REGCLDEF ) archi_regcldef_trim( c ) ; 
		else assert(0) ;
	}
}

static void archi_instrdef_trim( archi_ast_node *n )
{
	assert(0) ;
  assert( n->node_type == INSTRDEF ) ;
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
*/
}

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
/*
	archi_ast_node *c ;
	fctprop *p = (fctprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == ARGS )
			p->args = c->first_child ;
		else if( c->ntype == TID )
			p->name = (const char*)c->data ;
		else p->body = c ;
	}
*/
}

static void archi_auxsect_trim( archi_ast_node *n )
{
	assert( n->node_type == AUXSECT ) ;

	archi_ast_node *c ;
	FOREACH_CHILD( n, c ){
		if( c->node_type == FCTDEF ) archi_fctdef_trim( c ) ;
		else assert(0) ;
	}
}

void archi_ast_trim( archi_ast_node *n )
{
	assert( n->node_type == ARCHDEF ) ;

	archi_ast_node *c ;
	FOREACH_CHILD(n, c){
		switch( c->node_type ){
			case REGSECT: 	archi_regsect_trim( c ) ; break ;
			case INSTRSECT:	archi_instrsect_trim( c ) ; break ;
			case AUXSECT:		archi_auxsect_trim( c ) ; break ;
			default: assert(0) ;
		}	
	}
}
