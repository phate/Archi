#include "patternsect.h"
#include "../debug.h"

#include <string.h>

static void archi_matchdef_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( st && hf && sf && n && n->node_type == NT_MATCHDEF ) ;

  archi_ast_node *refnode = n->attr.nt_matchdef.refnode->attr.nt_refnode.instr ;

  fprintf( sf, "static bool jive_arch_match_%s( jive_graph *graph, jive_node *node ){\n", n->attr.nt_matchdef.id ) ;
  //fprintf( sf, "\tbool return = false ;" ) ;

  archi_ast_node *s ;
  FOREACH_PREV_SIBLING( refnode, s ){
    if( s == refnode ) continue ;

    #define X( jv_instr, ni, no, jvnodecl ) \
      if( !strcmp(#jv_instr, s->data_type) ) \
        fprintf( sf, "node = node")
        fprintf( sf, "\tif( node->type != &%s ) return false ;\n", #jvnodecl ) ; 
    
      JIVE_PREDEF_INSTR
    #undef X
    
//    fprintf( sf, "if( node->type != ) return false ;\n", s->data_type ) ;
  }
  
  FOREACH_NEXT_SIBLING( refnode, s ){
    fprintf( sf, "%s\n", s->data_type ) ;
  }

  fprintf( sf, "}\n\n" ) ;
}

void archi_patternsect_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( st && hf && sf && n && n->node_type == NT_PATTERNSECT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_MATCHDEF )
      archi_matchdef_generate( st, c, hf, sf ) ;
  }
}
