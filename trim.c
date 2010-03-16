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

#include <stdio.h>

static void trim_regcldef( node *n )
{
	assert( n->ntype == REGCLDEF ) ;

	node *c ;
	regclprop *p = (regclprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == BITS ){
			p->bits = *((int32_t*)c->data) ;
			destroy_node( c ) ; 
		}
		else if( c->ntype == REGS ){
			p->regs = c ;
			node *cc ;
			int32_t i = 0 ;
			FOREACH_CHILD( p->regs, cc ) i++ ;
			*((int32_t*)c->data) = i ; 
		}
		else if( c->ntype == ID )
			p->name = (const char*)c->data ;
	}
}

static void trim_regsect( node* n )
{
	node *c ;
	FOREACH_CHILD(n, c){
		if( c->ntype == REGDEF ) trim_regdef( c ) ;
		else if( c->ntype == REGCLDEF ) trim_regcldef( c ) ; 
		else assert(0) ;
	}
}

static void trim_instrdef( node *n )
{
	node *c ;
	instrprop *p = (instrprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == INPUT )
			p->input = c ;
		else if( c->ntype == OUTPUT )
			p->output = c ;
		else if( c->ntype == IMMEDIATE )
			p->immediates = c ;
		else if( c->ntype == ENCODING )
			p->encoding = c ;
		else if( c->ntype == ID )
			p->name = (const char*)c->data ;
	}
}

static void trim_instrsect( node *n )
{
	node *c ;
	FOREACH_CHILD(n, c){
		if( c->ntype == INSTRDEF ) trim_instrdef( c ) ; 
		else assert(0) ;
	}
}

static void trim_fctdef( node *n )
{
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
	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == FCTDEF ) trim_fctdef( c ) ;
		else assert(0) ;
	}
}

void trim_tree( node *n )
{
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
