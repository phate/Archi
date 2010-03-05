#include "analyze.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static void insert_node( symtab stab, const char* key, node* n, uint32_t linenr )
{
	node *l = lookup( stab, key ) ;
	if( l == NULL ){
		insert( stab, key, n ) ;
		return ;
	}
		
	char str[256] ;
	emsg *m1, *m2 ;
	if( !strcmp( l->dtype, n->dtype ) ){
		sprintf( str, "redeclaration of '%s'", key ) ;
		m1 = create_msg( linenr, str ) ;
	}
	else{
		sprintf( str, "conflicting types for '%s'", key ) ;
		m1 = create_msg( linenr, str ) ;
	}
	
	sprintf( str, "previous declaration of '%s' was here", key ) ;
	m2 = create_msg( l->linenr, str ) ;

	n->emsgs = append_msgs( n->emsgs, append_msgs( m1, m2 ) ) ;
}

void fill_symtab( symtab stab, node *n )
{
	if( n->ntype == REGDEF || n->ntype == REGCLDEF ||
			n->ntype == INSTRDEF )
	{
		node* c ;
		FOREACH_CHILD( n, c ){
			if( c->ntype == ID ){
				insert_node( stab, c->data, n, c->linenr ) ;
				destroy_node( c ) ;
				break ;
			}
		}
	}

	node *c ;
	FOREACH_CHILD(n,c){
		fill_symtab( stab, c ) ;
	}
}

static void tc_regdef( node *n )
{
	regprop *p = (regprop*)n->data ;
	if( p->code == -1 ){
		emsg *m = create_msg( n->linenr, "property 'code' was not defined" ) ; 
		n->emsgs = append_msgs( n->emsgs, m ) ;
	}
}

static void tc_regcldef( node* n )
{

}

static void tc_regsect( node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->ntype ){
			case REGDEF: tc_regdef( c ) ; break ;
			case REGCLDEF: tc_regcldef( c ) ; break ;
			default: assert(0) ;
		}
	}
}

static void tc_instrsect( node *n )
{

}

void typecheck( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->ntype ){
			case REGSECT: tc_regsect( c ) ; break ;
			case INSTRSECT:	tc_instrsect( c ) ; break ;
			default: assert(0) ;
		}
	}
}
