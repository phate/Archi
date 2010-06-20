#include "node.h"
#include "../debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*
instrprop* create_instrprop()
{
	instrprop *p = (instrprop*)malloc( sizeof(instrprop) ) ;

	p->input = NULL ;
	p->output = NULL ;
	p->immediates = NULL ;
	p->encoding = NULL ;
	p->name = NULL ;

	return p ;
}

fctprop* create_fctprop()
{
	fctprop *p = (fctprop*)malloc( sizeof(fctprop) ) ;

	p->args = NULL ;
	p->body = NULL ;
	p->name = NULL ;	

	return p ;
}
*/

/*
archi_ast_node* archi_ast_node_create( archi_ast_nodetype ntype, char* dtype, unsigned int linenr )
{
	archi_ast_node *n = malloc( sizeof(archi_ast_node) ) ;

	n->node_type = ntype ;
	n->data_type = dtype ;
	n->first_child = NULL ;
	n->last_child = NULL ;
	n->next_sibling = NULL ;
	n->prev_sibling = NULL ;
	n->parent = NULL ;
	n->linenr = linenr ;
	n->emsg_list = NULL ;

  memset( n->attr, 0, sizeof(node_attributes) ) ;

	return n ;
}
*/
static int archi_ast_node_destroy( archi_ast_node *n )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    talloc_free( c ) ;
  }

  if( n->prev_sibling ) n->prev_sibling->next_sibling = n->next_sibling ;
  else if( n->parent != NULL ) n->parent->first_child = n->next_sibling ;

  if( n->next_sibling ) n->next_sibling->prev_sibling = n->prev_sibling ;
  else if( n->parent != NULL ) n->parent->last_child = n->prev_sibling ;

  return 0 ;
}

archi_ast_node* archi_ast_node_talloc( TALLOC_CTX *ctx )
{
  archi_ast_node *n = talloc( ctx, archi_ast_node ) ;
  talloc_set_destructor( n, archi_ast_node_destroy ) ;

  return n ;
}

void archi_ast_node_init( archi_ast_node *n, archi_ast_nodetype ntype, const char* dtype, unsigned int linenr )
{
  DEBUG_ASSERT( n ) ;

  n->node_type = ntype ;
  n->data_type =  dtype == NULL ? NULL : talloc_strdup( n, dtype ) ;
  n->first_child = NULL ;
  n->last_child = NULL ;
  n->next_sibling = NULL ;
  n->prev_sibling = NULL ;
  n->parent = NULL ;
  n->linenr = linenr ;
  n->emsg_list = NULL ; 

  //FIXME: initialize union
}

static struct ntname { archi_ast_nodetype type; const char* name; } ntnames[] =
{
	{NT_ARCHDEF, "ArchDef"},
	{NT_REGSECT, "RegSect"},
	{INSTRSECT, "InstrSect"},
	{AUXSECT, "AuxSect"},
	{NT_REGDEF, "RegDef"},
	{NT_CODE, "Code"},
	{NT_REGCLDEF, "RegClDef"},
	{NT_REGS, "Regs"},
	{NT_BITS, "Bits"},
	{INSTRDEF, "InstrDef"},
	{INPUT, "Input"},
	{OUTPUT, "Output"},
	{IMMEDIATE, "Immediate"},
	{ENCODING, "Encoding"},
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
	{BOOLEAN, "Boolean"},
	{NT_ID, "ID"},
	{TID, "TID"}
} ;

static const char* ntnames_lookup( archi_ast_nodetype type )
{

	for( int i = 0, f = 0; f < sizeof(ntnames); f += sizeof(struct ntname), i++ ){
		if( type == ntnames[i].type ) return ntnames[i].name ;
	}

	return NULL ;
}

static void node2string( FILE* f, archi_ast_node *n )
{
	char str[256] ;

	const char* name = ntnames_lookup( n->node_type ) ;
	DEBUG_ASSERT( name ) ;
	
	switch( n->node_type ){
		case NT_CODE:
			sprintf( str, "%d", n->attr.code ) ; break ;
		case NT_REGS:
			sprintf( str, "%d", n->attr.nregs ) ; break ;
		case NT_BITS:
			sprintf( str, "%d", n->attr.bits ) ; break ;
		case NT_ID:
			sprintf( str, "%s", n->attr.id ) ; break ;
/*		case TID:
			sprintf( str, "%s %s", n->data_type, (const char*)n->data ) ; break ;
		case NUMBER:
			sprintf( str, "%s", (const char*)n->data ) ; break ;
		case BITSTRING:{
			size_t l = strlen( (const char *)n->data ) ;
			sprintf( str, "%s", strncat( str, ((const char*)n->data)+1, l-2) ) ; break ;}
*/		default:
			sprintf( str, "%s", "" ) ; break ;

	}
	
	const char* string = "\"n: %p\\np: %p\\nfc: %p\\nlc: %p\\nns: %p\\nps: %p\\n%s\\n%s\\nlnr:%d\"" ;					
	fprintf( f, string, n, n->parent, n->first_child,
		n->last_child, n->next_sibling, n->prev_sibling, name, str, n->linenr ) ;
}

static void write_node( FILE *f, archi_ast_node *p )
{
	archi_ast_node *c ;
	FOREACH_CHILD( p, c ){
		node2string( f, p ) ;
		fprintf( f, " -> " ) ;
		node2string( f, c ) ;
		fprintf( f, " ;\n" ) ;
		write_node( f, c ) ;
	}
}

void archi_view_ast( archi_ast_node* n )
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
