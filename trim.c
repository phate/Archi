#include "trim.h"

#include <stdlib.h>
#include <assert.h>

static void trim_regdef( node* n )
{
	node *c ;
	regprop *p = (regprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == CODE ){
			p->code = *((int32_t*)c->data) ;
			destroy_node( c ) ;
		}
		else if( c->ntype == ID )
			p->name = (const char*)c->data ;
	}
}

static void trim_regcldef( node *n )
{
	assert( n->ntype == REGCLDEF ) ;

	node *c ;
	regclprop *p = (regclprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == BITS ){
			if( p->bits == -1 ) 
				p->bits = *((int32_t*)c->data) ;
			else EMSG_MISSING_PROPERTY( n, "bits" ) ;
      destroy_node( c ) ;
		}
		else if( c->ntype == REGS ){
			if( p->regs == NULL ){
        p->regs = c ;
			  node *cc ;
			  int32_t i = 0 ;
			  FOREACH_CHILD( p->regs, cc ) i++ ;
			  *((int32_t*)c->data) = i ;
      }
		  else EMSG_MISSING_PROPERTY( n, "regs" ) ;
    }
		else if( c->ntype == ID )
			p->name = (const char*)c->data ;
	}
}

static void trim_regsect( node* n )
{
	assert( n->ntype == REGSECT ) ;

	node *c ;
	FOREACH_CHILD(n, c){
		if( c->ntype == REGDEF ) trim_regdef( c ) ;
		else if( c->ntype == REGCLDEF ) trim_regcldef( c ) ; 
		else assert(0) ;
	}
}

static void trim_instrdef( node *n )
{
	assert( n->ntype == INSTRDEF ) ;

	node *c ;
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

static void trim_instrsect( node *n )
{
	assert( n->ntype == INSTRSECT ) ;

	node *c ;
	FOREACH_CHILD(n, c){
		if( c->ntype == INSTRDEF ) trim_instrdef( c ) ; 
		else assert(0) ;
	}
}

static void trim_fctdef( node *n )
{
	assert( n->ntype == FCTDEF ) ;

	node *c ;
	fctprop *p = (fctprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == ARGS )
			p->args = c->first_child ;
		else if( c->ntype == TID )
			p->name = (const char*)c->data ;
		else p->body = c ;
	}
}

static void trim_auxsect( node *n )
{
	assert( n->ntype == AUXSECT ) ;

	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == FCTDEF ) trim_fctdef( c ) ;
		else assert(0) ;
	}
}

void trim_tree( node *n )
{
	assert( n->ntype == ARCHDEF ) ;

	node *c ;
	FOREACH_CHILD(n, c){
		switch( c->ntype ){
			case REGSECT: 	trim_regsect( c ) ; break ;
			case INSTRSECT:	trim_instrsect( c ) ; break ;
			case AUXSECT:		trim_auxsect( c ) ; break ;
			default: assert(0) ;
		}	
	}
}
