#include "trim.h"

#include <stdlib.h>
#include <assert.h>

static void trim_regdef( node* n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == CODE ){
			((regprop*)n->data)->code = *((int32_t*)c->data) ;
			destroy_node( c ) ;
		}
	}
}

static void trim_regcldef( node *n )
{
	node *c ;
	regclprop *p = (regclprop*)n->data ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == BITS ){
			p->bits = *((int32_t*)c->data) ;
			destroy_node( c ) ; 
		}
		else if( c->ntype == REGS ){
			p->regs = c->first_child ;
		} 
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
			p->input = c->first_child ;
		else if( c->ntype == OUTPUT )
			p->output = c->first_child ;
		else if( c->ntype == IMMEDIATE )
			p->immediates = c->first_child ;
	}
}

static void trim_instrsect( node *n )
{
	node* c ;
	FOREACH_CHILD(n, c){
		if( c->ntype == INSTRDEF ) trim_instrdef( c ) ; 
		else assert(0) ;
	}
}

void trim_tree( node *n )
{
	node *c ;
	FOREACH_CHILD(n, c){
		switch( c->ntype ){
			case REGSECT: 		trim_regsect( c ) ; break ;
			case INSTRSECT:		trim_instrsect( c ) ; break ;
			default: assert(0) ;
		}	
	}
}
