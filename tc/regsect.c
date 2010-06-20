#include "regsect.h"
#include "../debug.h"
#include "../ehandling.h"

#include <string.h>

static void archi_regdef_typecheck( archi_ast_node *n )
{
  DEBUG_ASSERT( n && n->node_type == NT_REGDEF ) ;

  if( n->attr.reg->code == ARCHI_REG_CODE_NOT_DEFINED ) EMSG_MISSING_ATTRIBUTE( n, "code" ) ;
}

static void archi_regcl_regs_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_REGS ) ;

  archi_ast_node *s ;
  FOREACH_SIBLING( n->first_child, s ){
    archi_ast_node* l = archi_symtab_lookup( st, s->attr.id ) ;
    if( l == NULL ) EMSG_MISSING_REGISTER( s, s->attr.id ) ;
    else if( strcmp(l->data_type, "Reg") ) EMSG_WRONG_TYPE( s, s->attr.id, "Reg" ) ;
  }
}

static void archi_regcldef_typecheck( archi_symtab *st, archi_ast_node *n )
{
  if( n->attr.regcl->bits == ARCHI_REGCL_BITS_NOT_DEFINED ) EMSG_MISSING_ATTRIBUTE( n, "bits" ) ;
  if( n->attr.regcl->regs == ARCHI_REGCL_REGS_NOT_DEFINED ) EMSG_MISSING_ATTRIBUTE( n, "regs" ) ;
  else archi_regcl_regs_typecheck( st, n->attr.regcl->regs ) ;
  //TODO check whether register classes are hierarchical
}

void archi_regsect_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_REGSECT ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    switch( c->node_type ){
      case NT_REGDEF:    archi_regdef_typecheck( c ) ; break ;
      case NT_REGCLDEF:  archi_regcldef_typecheck( st, c ) ; break ;
      default: DEBUG_ASSERT( 0 ) ;
    }
  }
}

