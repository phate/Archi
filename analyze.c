#include "analyze.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static void insert_node( symtab stab, node *n )
{
	const char* key = (const char*)n->data ;

	node *l = lookup( stab, key ) ;
	if( l == NULL ){
		insert( stab, key, n->parent ) ;
		return ;
	}
	
	if( !strcmp( l->dtype, n->parent->dtype ) )
		add_emsg( n, "redeclaration of '%s'", key ) ;
	else 
		add_emsg( n, "conflicting type for '%s'", key ) ;

	add_emsg( n, "previous declaration of '%s' was in line %d", key, l->linenr ) ;

}

void fill_symtab( symtab stab, node *n )
{
	if( n->ntype == REGDEF || n->ntype == REGCLDEF ||
			n->ntype == INSTRDEF || n->ntype == FCTDEF )
	{
		node *c ;
		FOREACH_CHILD( n, c ){
			switch( c->ntype ){
				case TID:	n->dtype == strdup( c->dtype ) ;
				case ID:	insert_node( stab, c ) ; break ;
				default: break ;
			}
		}
	}

	node *c ;
	FOREACH_CHILD(n,c){
		fill_symtab( stab, c ) ;
	}
}

static void msg_missing_property( node *n, const char* prop, const char* name )
{
	add_emsg( n, "property '%s' was not defined in '%s'", prop, name ) ;
}

static void tc_regdef( node *n )
{
	regprop *p = (regprop*)n->data ;
	if( p->code == -1 ) msg_missing_property( n, "code", (const char*)p->id->data ) ;
}

static void tc_regclregs( symtab stab, node *n )
{
	node *s ;
	FOREACH_SIBLING( n, s ){
		node* l = lookup( stab, (const char*)s->data ) ;
		if( l == NULL ) add_emsg( s, "register '%s' was not declared", (const char*)s->data ) ;
	}
}

static void tc_regcldef( symtab stab, node* n )
{
	regclprop *p = (regclprop*)n->data ;
	if( p->bits == -1 ) 	msg_missing_property( n, "bits", (const char*)p->id->data ) ;
	if( p->regs == NULL ) msg_missing_property( n, "regs", (const char*)p->id->data ) ;
	else tc_regclregs( stab, p->regs ) ;
}

static void tc_regsect( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->ntype ){
			case REGDEF: 		tc_regdef( c ) ; break ;
			case REGCLDEF: 	tc_regcldef( stab, c ) ; break ;
			default: assert(0) ;
		}
	}
}

static void tc_instrsect( node *n )
{

}

static void tc_auxsect( node *n )
{
}

void typecheck( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->ntype ){
			case REGSECT: tc_regsect( stab, c ) ; break ;
			case INSTRSECT:	tc_instrsect( c ) ; break ;
			case AUXSECT:		tc_auxsect( c ) ; break ;
			default: assert(0) ;
		}
	}
}
