#include "nodes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


regprop* create_regprop()
{
	regprop *p = (regprop*)malloc( sizeof(regprop) ) ;
	p->code = -1 ;

	return p ;
}

regclprop* create_regclprop()
{
	regclprop *p = (regclprop*)malloc( sizeof(regclprop) ) ;
	
	p->bits = -1 ;
	p->regs = NULL ;

	return p ;
}

instrprop* create_instrprop()
{
	instrprop *p = (instrprop*)malloc( sizeof(instrprop) ) ;

	p->input = NULL ;
	p->output = NULL ;
	p->immediates = NULL ;

	return p ;
}

fctprop* create_fctprop()
{
	fctprop *p = (fctprop*)malloc( sizeof(fctprop) ) ;

	p->args = NULL ;
	
	return p ;
}

node* create_node( nodetype ntype, char* dtype, void* data, unsigned int linenr )
{
	node *n = malloc( sizeof(node) ) ;

	n->ntype = ntype ;
	n->dtype = dtype ;
	n->first_child = NULL ;
	n->last_child = NULL ;
	n->next_sibling = NULL ;
	n->prev_sibling = NULL ;
	n->parent = NULL ;
	n->data = data ;
	n->linenr = linenr ;
	n->emsgs = NULL ;

	return n ;
}

void destroy_node( node* p )
{
	node *c ;
	FOREACH_CHILD( p, c ){
		destroy_node( c ) ;
	}

	if( p->prev_sibling ) p->prev_sibling->next_sibling = p->next_sibling ;
	else p->parent->first_child = p->next_sibling ;

	if( p->next_sibling ) p->next_sibling->prev_sibling = p->prev_sibling ;
	else p->parent->last_child = p->prev_sibling ;


	//TODO free emsgs
	free( p->dtype ) ; 
	free( p->data ) ;	
	free( p ) ;
}

static struct ntname { nodetype type; const char* name; } ntnames[] =
{
	{ARCHDEF, "ArchDef"},
	{REGSECT, "RegSect"},
	{INSTRSECT, "InstrSect"},
	{AUXSECT, "AuxSect"},
	{REGDEF, "RegDef"},
	{CODE, "Code"},
	{REGCLDEF, "RegClDef"},
	{REGS, "Regs"},
	{BITS, "Bits"},
	{INSTRDEF, "InstrDef"},
	{INPUT, "Input"},
	{OUTPUT, "Output"},
	{IMMEDIATE, "Immediate"},
	{FCTDEF, "FctDef"},
	{ARGS, "Args"},
	{IFTHENELSE, "ITE"},
	{LOGICALOR, "||"},
	{LOGICALAND, "&&"},
	{NOTEQUAL, "!="},
	{EQUAL, "=="},
	{LESSTHAN, "<"},
	{GREATERTHAN, ">"},
	{LESSTHANEQUAL, "<="},
	{GREATERTHANEQUAL, ">="},
	{SHIFTRIGHT, ">>"},
	{SHIFTLEFT, "<<"},
	{PLUS, "+"},
	{MINUS, "-"},
	{TIMES, "*"},
	{DIVIDE, "/"},
	{MOD, "%"},
	{CONCATENATION, "++"},
	{BITSLICE, "[ ]"},
	{PROPSELECTION, "."},
	{FCTCALL, "FctCall()"},
	{NUMBER, "Number"},
	{BITSTRING, "Bitstring"},
	{ID, "ID"},
	{TID, "TID"}
} ;

static const char* ntnames_lookup( nodetype type )
{

	for( int i = 0, f = 0; f < sizeof(ntnames); f += sizeof(struct ntname), i++ ){
		if( type == ntnames[i].type ) return ntnames[i].name ;
	}

	return NULL ;
}

static void node2string( FILE* f, node* n )
{
	char str[256] ;

	const char* name = ntnames_lookup( n->ntype ) ;
	if( name == NULL ) assert(0) ;
	
	switch( n->ntype ){
		case CODE:
			sprintf( str, "%ld", *((long*)n->data) ) ; break ;
		case BITS:
			sprintf( str, "%ld", *((long*)n->data) ) ; break ;
		case ID:
			sprintf( str, "%s", (const char*)n->data ) ; break ;
		case TID:
			sprintf( str, "%s %s", n->dtype, (const char*)n->data ) ; break ;
		case NUMBER:
			sprintf( str, "%s", (const char*)n->data ) ; break ;
		case BITSTRING:{
			size_t l = strlen( (const char *)n->data ) ;
			sprintf( str, "%s", strncat( str, ((const char*)n->data)+1, l-2) ) ; break ;}
		default:
			sprintf( str, "%s", "" ) ; break ;
	}
	
	const char* string = "\"n: %p\\np: %p\\nfc: %p\\nlc: %p\\nns: %p\\nps: %p\\n%s\\n%s\\nlnr:%d\"" ;					
	fprintf( f, string, n, n->parent, n->first_child,
		n->last_child, n->next_sibling, n->prev_sibling, name, str, n->linenr ) ;
}

static void write_node( FILE* f, node* p )
{
	node *c ;
	FOREACH_CHILD( p, c ){
		node2string( f, p ) ;
		fprintf( f, " -> " ) ;
		node2string( f, c ) ;
		fprintf( f, " ;\n" ) ;
		write_node( f, c ) ;
	}
}

void view_tree( node* n )
{
	if( !n ) return ;

	FILE* f = fopen( "/tmp/graph.dot", "w" ) ;
	if( !f ){ 
		fprintf( stderr, "Error: Cannot create file /tmp/graph.dot.\n" ) ;
		return ;
	}

	fprintf( f, "digraph G{\n") ;
	write_node( f, n ) ;
	fprintf( f, "}\n" ) ;
	fclose( f ) ;

	system( "rm -f /tmp/graph.ps && dot -Tps /tmp/graph.dot >> /tmp/graph.ps && evince /tmp/graph.ps " ) ;
}
