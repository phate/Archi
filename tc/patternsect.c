#include "patternsect.h"
#include "typecheck.h"
#include "../ast/node.h"
#include "../debug.h"

static void archi_matchdef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_MATCHDEF ) ;


  if( n->attr.nt_matchdef.input != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_create( NULL ) ;
    l = archi_symtab_idlist_fill( l, st, NT_REGCLDEF ) ;
    archi_tidlist_typecheck( st, n->attr.nt_matchdef.input->first_child, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "input" ) ;

  if( n->attr.nt_matchdef.output != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_create( NULL ) ;
    l = archi_symtab_idlist_fill( l, st, NT_REGCLDEF ) ;
    archi_tidlist_typecheck( st, n->attr.nt_matchdef.output->first_child, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "output" ) ;

  if( n->attr.nt_matchdef.ipattern == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "ipattern" ) ;

  if( n->attr.nt_matchdef.opattern == NULL )
    EMSG_MISSING_ATTRIBUTE( n, "opattern" ) ;
}

void archi_patternsect_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_PATTERNSECT ) ;
  
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_MATCHDEF ){
      archi_symtab_push_scope( st ) ;
      archi_matchdef_typecheck( st, c ) ;
      archi_symtab_pop_scope( st ) ;
    }
    else DEBUG_ASSERT(0) ;
  }
}
