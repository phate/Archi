#include "typecheck.h"
#include "../ehandling.h"
#include "trim.h"
#include "regsect.h"
#include "instrsect.h"
#include "../debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static void archi_typecheck_init( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_ARCHDEF ) ;

  archi_ast_node *is = archi_ast_node_create( n, NT_INTERNALSECT, NULL, 0 ) ;
  archi_ast_node_first_child_set( n, is ) ;

  #define JIVE_PREDEF_INSTR \
    X( JVBitconstant, 0, 1 ) \
    X( JVAdd, 2, 1 )
  
  archi_ast_node *tmp ;
  #define X( jv_instr, ni, no ) \
    tmp = archi_ast_node_create( n, NT_JVINSTRDEF, "Instruction", 0 ) ; \
    tmp->attr.nt_jvinstrdef.id = talloc_strdup( tmp, #jv_instr ) ; \
    tmp->attr.nt_jvinstrdef.ninputs = ni ; \
    tmp->attr.nt_jvinstrdef.noutputs = no ; \
    archi_ast_node_first_child_set( is, tmp ) ;

    JIVE_PREDEF_INSTR
  #undef X
  #undef JIVE_PREDEF_INSTR
}

static void archi_node_insert( archi_symtab *st, const char *key, archi_ast_node *n )
{
  archi_ast_node *l = archi_symtab_lookup( st, key ) ;
  if( l == NULL ){
    archi_symtab_insert( st, key, n ) ;
    return ;
  }
	
  if( !strcmp( l->data_type, n->data_type ) )
		EMSG_REDECLARATION( n, key ) ; 
	else 
		archi_add_emsg( n, "conflicting type for '%s'", key ) ;

	EMSG_PREVIOUS_DECLARATION( n, key, l->linenr ) ;
}

static void archi_symtab_toplevel_fill( archi_symtab *st, archi_ast_node *n )
{
  switch( n->node_type ){
    case NT_REGDEF:
      archi_node_insert( st, n->attr.nt_regdef.id, n ) ;
      break ;
    case NT_REGCLDEF:
      archi_node_insert( st, n->attr.nt_regcldef.id, n ) ;
      break ;
    case NT_INSTRDEF:
      archi_node_insert( st, n->attr.nt_instrdef.id, n ) ;
      break ;
    case NT_JVINSTRDEF:
      archi_node_insert( st, n->attr.nt_jvinstrdef.id, n ) ;
      break ;
    default: break ;
  }

  archi_ast_node *c ;
  FOREACH_CHILD(n, c){
    archi_symtab_toplevel_fill( st, c ) ;
  }
}

/*
static void msg_missing_property( node *n, const char* prop, const char* name )
{
	add_emsg( n, "property '%s' was not defined in '%s'", prop, name ) ;
}

static void tc_regdef( node *n )
{
	assert( n->ntype == REGDEF ) ;

	regprop *p = (regprop*)n->data ;
	if( p->code == -1 ) msg_missing_property( n, "code", p->name ) ;

}

static void tc_regclregs( symtab stab, node *n )
{
	assert( n->ntype == REGS ) ;

	node *s ;
	FOREACH_SIBLING( n->first_child, s ){
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
*/
void archi_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_ARCHDEF ) ;
  

  archi_ast_trim( n ) ; 
  archi_typecheck_init( st, n ) ;
  archi_symtab_toplevel_fill( st, n ) ;

	archi_ast_node *c ;
	FOREACH_CHILD( n, c ){
		switch( c->node_type ){
			case NT_REGSECT:    archi_regsect_typecheck( st, c ) ; break ;
			case NT_INSTRSECT:  archi_instrsect_typecheck( st, c ) ; break ;
      case NT_INTERNALSECT: break ;
      case NT_PATTERNSECT: break ;
	//		case AUXSECT:		tc_auxsect( stab, c ) ; break ;
			default: DEBUG_ASSERT( 0 ) ;
		}
	}

}
