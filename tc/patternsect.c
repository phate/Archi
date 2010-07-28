#include "patternsect.h"
#include "typecheck.h"
#include "../ast/node.h"
#include "../debug.h"

static void archi_ipattern_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_IPATTERN ) ;

  archi_symtab_idlist *tf ;
  archi_symtab_idlist *tl = archi_symtab_idlist_fill( NULL, st, NT_JVINSTRDEF ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    DEBUG_ASSERT( c->node_type == NT_NODEDEF ) ;
    tf = archi_variabledef_typecheck( st, c, c->attr.nt_nodedef.id, tl ) ; 

    if( tf ){
      archi_ast_node *l = archi_symtab_lookup( st, archi_symtab_idlist_id(tf) ) ;
      DEBUG_ASSERT(l) ;

      int32_t niex = l->attr.nt_jvinstrdef.ninputs ;
      int32_t nig = c->attr.nt_nodedef.input->attr.nt_input.nchildren ;
      int32_t noex = l->attr.nt_jvinstrdef.noutputs ;
      int32_t nog = c->attr.nt_nodedef.output->attr.nt_output.nchildren ;
      if( niex !=  nig )
        archi_add_emsg( c, "expected %d inputs, got %d inputs", niex, nig ) ;

      if( noex != nog  ) 
        archi_add_emsg( c, "expected %d outputs, got %d outputs", noex, nog ) ;
    }
  } 

  TALLOC_FREE( tl ) ;
}

static void archi_matchdef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_MATCHDEF ) ;


  if( n->attr.nt_matchdef.input != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_fill( NULL, st, NT_REGCLDEF ) ;
    archi_ast_node *c ;
    FOREACH_CHILD( n->attr.nt_matchdef.input, c )
      archi_variabledef_typecheck( st, c, c->attr.nt_tid.id, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "input" ) ;

  if( n->attr.nt_matchdef.output != NULL ){
    archi_symtab_idlist *l = archi_symtab_idlist_fill( NULL, st, NT_REGCLDEF ) ;
    archi_ast_node *c ;
    FOREACH_CHILD( n->attr.nt_matchdef.output, c )
      archi_variabledef_typecheck( st, c, c->attr.nt_tid.id, l ) ;
    TALLOC_FREE( l ) ;
  }
  else EMSG_MISSING_ATTRIBUTE( n, "output" ) ;

  if( n->attr.nt_matchdef.ipattern != NULL )
    archi_ipattern_typecheck( st, n->attr.nt_matchdef.ipattern ) ;
  else EMSG_MISSING_ATTRIBUTE( n, "ipattern" ) ;

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
