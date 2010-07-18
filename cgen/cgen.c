#include "cgen.h"
#include "regsect.h"
#include "instrsect.h"
#include "../debug.h"

/*
static void gc_regdef( node *n, FILE *hf, FILE *sf )
{
	regprop *p = (regprop*)n->data ;

	fprintf( sf, "static const machine_register %s={ .name=\"%s\", .code=%d} ;\n",
		p->name, p->name, p->code ) ;	
}

static void gc_regcldef( node *n, FILE *hf, FILE *sf )
{
	regclprop *p = (regclprop*)n->data ;

	fprintf( sf, "const register_class %s={", p->name ) ;
	fprintf( sf, "\t.bits=%d", p->bits ) ;
	fprintf( sf, "\t.name=\"%s\"", p->name ) ;
	fprintf( sf, "} ;" ) ;
}

static void gc_regsect( node *n, FILE *hf, FILE *sf )
{
	node *c ;
	FOREACH_CHILD( n, c ){
		if( n->ntype == REGDEF )
			gc_regdef( c, hf, sf ) ;	
	}

	FOREACH_CHILD( n, c ){
		if( n->ntype == REGCLDEF )
			gc_regcldef( c, hf, sf ) ;
	}
}

static void gc_instrsect( node *c )
{

}

static void gc_auxsect( node *c )
{
}
*/

void archi_code_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ;

  archi_regsect_generate( st, n->attr.nt_archdef.regsect, hf, sf ) ;
  archi_instrsect_generate( n->attr.nt_archdef.instrsect, hf, sf ) ;
}
