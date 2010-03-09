#include "analyze.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static void insert_node( symtab stab, node *n, const char* prefix )
{

	const char* suffix = "" ;
	if( strcmp( prefix, "" ) ) suffix = "::" ;
	const char* postfix = (const char*)n->data ;

	char key[strlen(prefix)+strlen(suffix)+strlen(postfix)+1] ;
	sprintf( key, "%s%s%s", prefix, suffix, postfix ) ;

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

static void fill_symtab_( symtab stab, node *n, const char* prefix )
{
	if( n->ntype == REGDEF || n->ntype == REGCLDEF ||
			n->ntype == INSTRDEF || n->ntype == FCTDEF ||
			n->ntype == INPUT || n->ntype == OUTPUT ||
			n->ntype == IMMEDIATE || n->ntype == ARGS )
	{
		node *c ;
		FOREACH_CHILD( n, c ){
			switch( c->ntype ){
				case TID:	n->dtype == strdup( c->dtype ) ;
				case ID:	insert_node( stab, c, prefix ) ; break ;
				default: break ;
			}
		}
	}

	node *c ;
	FOREACH_CHILD(n,c){
		const char* pref = "" ;
		switch( n->ntype ){
			case INSTRDEF: pref = ((instrprop*)n->data)->name ; break ;
			case FCTDEF: pref = ((fctprop*)n->data)->name ; break ;
			default: break ; 
		}

		fill_symtab_( stab, c, pref ) ;
	}
}

void fill_symtab( symtab stab, node *n )
{
	fill_symtab_( stab, n, "" ) ;
}

static void msg_missing_property( node *n, const char* prop, const char* name )
{
	add_emsg( n, "property '%s' was not defined in '%s'", prop, name ) ;
}

static void tc_regdef( node *n )
{
	regprop *p = (regprop*)n->data ;
	if( p->code == -1 ) msg_missing_property( n, "code", p->name ) ;

}

static void tc_regclregs( symtab stab, node *n )
{
	node *s ;
	FOREACH_SIBLING( n, s ){
		const char *name = s->data ;
		node* l = lookup( stab, name ) ;
		if( l == NULL ) add_emsg( s, "register '%s' was not declared", name ) ;
		else if( strcmp(l->dtype, "Reg") )
			add_emsg( s, "'%s' is not of type 'Reg'", name ) ;
	}
}

static void tc_regcldef( symtab stab, node* n )
{
	regclprop *p = (regclprop*)n->data ;
	if( p->bits == -1 ) 	msg_missing_property( n, "bits", p->name ) ;
	if( p->regs == NULL ) msg_missing_property( n, "regs", p->name ) ;
	else tc_regclregs( stab, p->regs ) ;
	//TODO check whether register classes are hierarchical
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

static void tc_instrio( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		const char *type = c->dtype ;
		node* l = lookup( stab, type ) ;
		if( l == NULL )
			add_emsg( c, "register class '%s' was not declared", type ) ;
		else if( strcmp(l->dtype, "RegClass") )
			add_emsg( c, "'%s' is not of type 'RegClass'", (const char*)c->data ) ;
	}
}

static void tc_instrimmediates( node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( strcmp( c->dtype, "Int") )
			add_emsg( c, "'%s' is not of type 'Int'", (const char*)c->data ) ;
	}
}

static void tc_instrdef( symtab stab, node *n )
{
	instrprop *p = (instrprop*)n->data ;
	if( p->input == NULL ) msg_missing_property( n, "input", p->name ) ;
	else tc_instrio( stab, p->input ) ;

	if( p->output == NULL ) msg_missing_property( n, "output", p->name ) ;
	else tc_instrio( stab, p->output ) ;

	if( p->immediates == NULL ) msg_missing_property( n, "immediate", p->name ) ;
	else tc_instrimmediates( p->immediates ) ;
}

static void tc_instrsect( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == INSTRDEF ) tc_instrdef( stab, c ) ;
		else assert(0) ;
	}
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
			case INSTRSECT:	tc_instrsect( stab, c ) ; break ;
			case AUXSECT:		tc_auxsect( c ) ; break ;
			default: assert(0) ;
		}
	}
}
