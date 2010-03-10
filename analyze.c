#include "analyze.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static void insert_node( symtab stab, node *n, const char* name, const char* prefix )
{

	MANGLE_NAME( key, prefix, name ) ;
	
	node *l = lookup( stab, key ) ;
	if( l == NULL ){
		insert( stab, key, n ) ;
		return ;
	}
	
	if( !strcmp( l->dtype, n->dtype ) )
		add_emsg( n, "redeclaration of '%s'", key ) ;
	else 
		add_emsg( n, "conflicting type for '%s'", key ) ;

	add_emsg( n, "previous declaration of '%s' was in line %d", key, l->linenr ) ;

}

static void fill_symtab_( symtab stab, node *n, const char* prefix )
{
	switch( n->ntype ){
		case FCTDEF:
			n->dtype = strdup( n->first_child->dtype ) ;
		case REGDEF:
		case REGCLDEF:
		case INSTRDEF:
			insert_node( stab, n, n->first_child->data, prefix ) ;	
			break ;
		case ARGS:
		case INPUT:
		case OUTPUT:
		case IMMEDIATE:{
			node *c ;
			FOREACH_CHILD( n, c ) insert_node( stab, c, c->data, prefix ) ;
			break ;}
		default: break ;
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


static void tc_fctargs( symtab stab, node *n )
{
	node *s ;
	FOREACH_SIBLING( n, s ){
		if( !strcmp(s->dtype, "Bool") ) ;
		else if( !strcmp(s->dtype, "Int") ) ;
		else if( !strcmp(s->dtype, "Bits") ) ;
		else{
			node *l = lookup( stab, s->dtype ) ;
			if( l == NULL )
				add_emsg( s, "register class '%s' was not declared", s->dtype ) ;
			else if( strcmp(l->dtype, "RegClass") )
				add_emsg( s, "'%s' is not of type 'RegClass'", (const char*)s->data ) ;
		}
	}
}

static void msg_type_mismatch( node *n, const char* expected, const char* inferred )
{
	add_emsg( n, "type mismatch: expected type '%s', inferred type '%s'", expected, inferred ) ;
}

static void checkExp( symtab stab, node *n, const char* type, const char* prefix, node *f ) ;

static const char* inferExp( symtab stab, node *n, const char* prefix, node *f )
{
	switch( n->ntype ){
		case IFTHENELSE:{
			checkExp( stab, n->first_child, "Bool", prefix, f ) ;
			const char* t1 = inferExp( stab, n->first_child->next_sibling, prefix, f ) ;
			const char* t2 = inferExp( stab, n->last_child, prefix, f ) ;
			if( strcmp(t1, t2) ) msg_type_mismatch( n->first_child->next_sibling, t1, t2 ) ;	
			return t1 ;}
		case LOGICALOR:
		case LOGICALAND:{
			checkExp( stab, n->first_child, "Bool", prefix, f ) ;
			checkExp( stab, n->last_child, "Bool", prefix, f ) ;
			return "Bool" ;}
		case EQUAL:
		case NOTEQUAL:
		case LESSTHAN:
		case GREATERTHAN:
		case LESSTHANEQUAL:
		case GREATERTHANEQUAL:{
			const char* t1 = inferExp( stab, n->first_child, prefix, f ) ;
			const char* t2 = inferExp( stab, n->last_child, prefix, f ) ;
			if( strcmp(t1, t2) ) msg_type_mismatch( n->first_child, t1, t2 ) ;
			return "Bool" ;}
		case MOD:
		case PLUS:
		case MINUS:
		case TIMES:
		case DIVIDE:
		case SHIFTRIGHT:
		case SHIFTLEFT:{
			checkExp( stab, n->first_child, "Int", prefix, f ) ;
			checkExp( stab, n->last_child, "Int", prefix, f ) ;
			return "Int" ;}
		case CONCATENATION:{
			checkExp( stab, n->first_child, "Bits", prefix, f ) ;
			checkExp( stab, n->last_child, "Bits", prefix, f ) ;
			return "Bits" ;}
		case BITSLICE:{
			checkExp( stab, n->first_child, "Int", prefix, f ) ;
			checkExp( stab, n->first_child->next_sibling, "Int", prefix, f ) ;
			checkExp( stab, n->last_child, "Int", prefix, f ) ;
			return "Bits" ;}
		case PROPSELECTION:{
			const char *v = (const char*)n->first_child->data ;
			const char *p = (const char*)n->last_child->data ;
			MANGLE_NAME( key, prefix, v ) ;
			node *l = lookup( stab, key ) ;
			if( l == NULL )
				add_emsg( n, "'%s' undeclared", v ) ;
			else{
				l = lookup( stab, l->dtype ) ;
				if( l == NULL )
					add_emsg( n, "'%s' is not of type RegClass", v )  ;
			}
		
			if( strcmp( p, "code") )
				add_emsg( n, "'%s' has no property '%s'", v, p ) ;
			return n->last_child->dtype ;}
		case FCTCALL:{
			const char *fct = n->first_child->data ;
			node *l = lookup( stab, fct ) ;
			if( l == NULL ){
				add_emsg( n, "'%s' undeclared", fct ) ;
				return "Int" ; //TODO: output emsg, return value defaults to Int
			}
			else if( l->ntype != FCTDEF ){
				add_emsg( n, "'%s' is not a function", fct ) ;
				return "Int" ;//TODO: output emsg, return value defaults to Int
			}
	
			//check for forward declaration	
			if(f != NULL){
				node *s ;
				FOREACH_SIBLING( l->next_sibling, s ) if( f == s ) break ;
	
				if( s == NULL ){
					add_emsg( n, "'%s' undeclared", fct ) ;
					return l->dtype ;
				}
			}

			//check types of arguments
			unsigned int i = 1 ;
			node *al, *ag = ((fctprop*)l->data)->args ;
			FOREACH_SIBLING( n->first_child->next_sibling, al ){
				if( ag != NULL ){
					const char *type = inferExp( stab, al, prefix, f ) ;
					if( strcmp(type, ag->dtype) )
						add_emsg( n, "type mismatch at argument %d: expected '%s', inferred '%s'", i, ag->dtype, type ) ;
				}
				else{
					add_emsg( n, "function call '%s' has invalid number of arguments", fct ) ;
					return l->dtype ;
				}
				i++ ;
				ag = ag->next_sibling ;
			}
	
			if( ag != al )
				add_emsg( n, "function call '%s' has invalid number of arguments", fct ) ;

			return l->dtype ;}
		case NUMBER:
			return n->dtype ;
		case BITSTRING:
			return n->dtype ;
		case BOOLEAN:
			return n->dtype ;
		case ID:{
			const char* name = (const char*)n->data ;
			MANGLE_NAME( key, prefix, name ) ;
			node *l = lookup( stab, key ) ;
			if( l == NULL ){
				add_emsg( n, "'%s' undeclared", name ) ;
				return "Int" ; //TODO: write message that it defaults to int
			} 
		
			return l->dtype ;}
		default: assert(0) ;	
	}
}

static void checkExp( symtab stab, node *n, const char* type, const char* prefix, node *f )
{
	const char* tmp = inferExp( stab, n, prefix, f ) ;
	if( strcmp( tmp, type ) ){
		msg_type_mismatch( n, type, tmp ) ;
	}
}

static void tc_fctdef( symtab stab, node *n )
{
	fctprop *p = (fctprop*)n->data ;
	if( p->args != NULL ) tc_fctargs( stab, p->args ) ;

	const char* type = inferExp( stab, p->body, p->name, n ) ;

	if( strcmp(type, n->dtype) )
		msg_type_mismatch( n, n->dtype, type ) ;
}

static void tc_auxsect( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == FCTDEF ) tc_fctdef( stab, c ) ;
		else assert(0) ;
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

static void tc_instrencoding( symtab stab, node *n, const char* prefix )
{
	const char* type = inferExp( stab, n->first_child, prefix, NULL ) ;
	if( strcmp(type, "Bits") )
		msg_type_mismatch( n, "Bits", type ) ;
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

	if( p->encoding == NULL ) msg_missing_property( n, "encoding", p->name ) ;
	else tc_instrencoding( stab, p->encoding, p->name ) ;
}

static void tc_instrsect( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( c->ntype == INSTRDEF ) tc_instrdef( stab, c ) ;
		else assert(0) ;
	}
}

void typecheck( symtab stab, node *n )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->ntype ){
			case REGSECT: tc_regsect( stab, c ) ; break ;
			case INSTRSECT:	tc_instrsect( stab, c ) ; break ;
			case AUXSECT:		tc_auxsect( stab, c ) ; break ;
			default: assert(0) ;
		}
	}
}
