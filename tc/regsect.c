#include "regsect.h"
#include "../debug.h"
#include "../ehandling.h"

#include <string.h>

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

static void archi_regcls_sort( archi_ast_node* regcls[], uint32_t nregcls )
{
  archi_ast_node *tmp ;
  for( uint32_t i = 0; i < nregcls; i++ ){
    for( uint32_t f = i+1; f < nregcls; f++ ){
      uint32_t inregs = regcls[i]->attr.regcl->regs->attr.nregs ;
      uint32_t fnregs = regcls[f]->attr.regcl->regs->attr.nregs ; 
      if( fnregs < inregs ){
        tmp = regcls[i] ;
        regcls[i] = regcls[f] ;
        regcls[f] = tmp ; 
      }
    }
  }
}

static int32_t archi_regcls_disjoint( archi_ast_node *regcl0, archi_ast_node *regcl1 )
{
  archi_ast_node *c0, *c1 ;
  FOREACH_CHILD( regcl0->attr.regcl->regs, c0 ){
    FOREACH_CHILD( regcl1->attr.regcl->regs, c1 ){
      if( strcmp(c0->attr.id, c1->attr.id) == 0 ) return -1 ; 
    }
  }  

  return 0 ;
}

//checks whether regcls0 is a subset of regcls1
//assumes that regcls0 has less than or equal number of registers than regcls1
static int32_t archi_regcls_subset( archi_ast_node *regcl0, archi_ast_node *regcl1 )
{
  archi_ast_node *c0, *c1 ;
  FOREACH_CHILD( regcl0->attr.regcl->regs, c0 ){
    uint32_t reg_found = 0 ;
    FOREACH_CHILD( regcl1->attr.regcl->regs, c1 ){
      if( strcmp(c0->attr.id, c1->attr.id) == 0 ) reg_found = 1 ; 
    }
    if( !reg_found ) return -1 ;
  }

  return 0 ; 
}

static void archi_regcls_hierarchical( archi_symtab *st, archi_ast_node* regcls[], uint32_t nregcls )
{
  archi_regcls_sort( regcls, nregcls ) ;

  for( uint32_t i = 0; i < nregcls; i++ ){
    for( uint32_t f = i+1; f < nregcls; f++ ){
      uint32_t inregs = regcls[i]->attr.regcl->regs->attr.nregs ;
      uint32_t fnregs = regcls[f]->attr.regcl->regs->attr.nregs ; 
      if( archi_regcls_disjoint( regcls[i], regcls[f] ) ){
        if( !archi_regcls_subset( regcls[i], regcls[f] ) && inregs != fnregs ){
          regcls[i]->attr.regcl->pregcl = regcls[f] ;
          break ;
        }
        else EMSG_REGCLS_NOT_HIERARCHICAL( regcls[i], regcls[f] ) ;
      }      
    }
    
    archi_ast_node *c ;
    FOREACH_CHILD( regcls[i]->attr.regcl->regs, c ){
      archi_ast_node* l = archi_symtab_lookup( st, c->attr.id ) ;
      if( l != NULL && !strcmp(l->data_type, "Reg") && l->attr.reg->regcl == NULL ) l->attr.reg->regcl = regcls[i] ;
    }
  }
}

void archi_regsect_typecheck( archi_symtab *st, archi_ast_node *n )
{
  DEBUG_ASSERT( st && n && n->node_type == NT_REGSECT ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  uint32_t regcl_regs_defined = 1 ;
  archi_ast_node *regcls[n->attr.nregcls] ;
  FOREACH_CHILD( n, c ){
    switch( c->node_type ){
      case NT_REGDEF:    
        if( c->attr.reg->code == -1 ) EMSG_MISSING_ATTRIBUTE( c, "code" ) ;
        break ;  
    case NT_REGCLDEF:
        regcls[i++] = c ;
        if( c->attr.regcl->bits == -1 ) EMSG_MISSING_ATTRIBUTE( c, "bits" ) ;
        if( c->attr.regcl->regs == NULL ){
          regcl_regs_defined = 0 ;
          EMSG_MISSING_ATTRIBUTE( c, "regs" ) ;
        }
        else archi_regcl_regs_typecheck( st, c->attr.regcl->regs ) ;
        break ;
      default: DEBUG_ASSERT( 0 ) ;
    }
  }
  
  if( regcl_regs_defined == 1 ) archi_regcls_hierarchical( st, regcls, n->attr.nregcls ) ;

  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGDEF && c->attr.reg->regcl == NULL )
      EMSG_REGISTER_NOT_USED( c, c->attr.reg->id ) ;
  }
}

