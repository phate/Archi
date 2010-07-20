#include "cgen.h"
#include "prepare.h"
#include "regsect.h"
#include "instrsect.h"
#include "../debug.h"

void archi_code_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( n && n->node_type == NT_ARCHDEF ) ;

  archi_code_generate_prepare( n ) ;

  archi_regsect_generate( st, n->attr.nt_archdef.regsect, hf, sf ) ;
  archi_instrsect_generate( n->attr.nt_archdef.instrsect, hf, sf ) ;
}
