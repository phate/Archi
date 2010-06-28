#include "regsect.h"
#include "../debug.h"

static void archi_reg_defs_generate_( archi_ast_node *n, FILE *hf, uint32_t *i )
{
  archi_ast_node *c, *cc ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_reg_defs_generate_( c, hf, i );
    if( c->node_type == NT_REGS ){
      FOREACH_CHILD( c, cc ){
        fprintf( hf, "\tjive_arch_%s = %d,\n", cc->attr.id, (*i)++ ) ;
      }
    }
  }
}

static void archi_reg_defs_generate( archi_ast_node *n, FILE *hf )
{
  fprintf( hf, "typedef enum {\n" ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_reg_defs_generate_( c, hf, &i ) ;
  }

  fprintf( hf, "} jive_arch_reg_index ;\n\n" ) ;
}

static void archi_regcl_defs_generate_( archi_ast_node *n, FILE *hf, uint32_t *i )
{
  fprintf( hf, "\tjive_arch_%s = %d,\n", n->attr.regcl->id, (*i)++ ) ;
  
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_regcl_defs_generate_( c, hf, i ) ;
  }
}

static void archi_regcl_defs_generate( archi_ast_node *n, FILE *hf )
{
  fprintf( hf, "typedef enum {\n" ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_regcl_defs_generate_( c, hf, &i ) ;
  }

  fprintf( hf, "} jive_arch_regcls_index ;\n\n" ) ;

}

static void archi_reg_code_generate_( archi_symtab *st, archi_ast_node *n, FILE *sf )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_reg_code_generate_( st, c, sf ) ;    
  }

  FOREACH_CHILD( n->attr.regcl->regs, c ){
    archi_ast_node *l = archi_symtab_lookup( st, c->attr.id ) ;
    fprintf( sf, "\t[jive_arch_%s] = {\n", l->attr.reg->id ) ;
    fprintf( sf, "\t\t.name = \"%s\",\n", l->attr.reg->id ) ;
    fprintf( sf, "\t\t.regcls = &jive_arch_regcls[jive_arch_%s],\n", l->attr.reg->regcl->attr.regcl->id ) ;
    fprintf( sf, "\t\t.code = %d,\n", l->attr.reg->code ) ;
    fprintf( sf, "\t\t.index = jive_arch_%s,\n", l->attr.reg->id ) ;
    fprintf( sf, "\t\t.class_mask = " ) ;
    archi_ast_node *r = l->attr.reg->regcl ;
    while( r ){
      fprintf( sf, "(1 << jive_arch_%s)", r->attr.regcl->id ) ;
      r = r->attr.regcl->pregcl ;
      if( r ) fprintf( sf, " | " ) ;
    }
    fprintf( sf, "\n\t},\n" ) ;
  }
}

static void archi_reg_code_generate( archi_symtab *st, archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_cpureg jive_arch_regs [] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_reg_code_generate_( st, c, sf ) ;
  }

  fprintf( sf, "} ;\n\n" ) ;
}

static  archi_ast_node* archi_regcl_regs_index_node_find( archi_ast_node *n )
{
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF )
      return archi_regcl_regs_index_node_find( c ) ;
  }

  return n->attr.regcl->regs->first_child ;
} 

static void archi_regcl_code_generate_( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "\t[jive_arch_%s] = {\n", n->attr.regcl->id ) ;
  fprintf( sf, "\t\t.name = \"%s\",\n", n->attr.regcl->id ) ;
  fprintf( sf, "\t\t.nbits = %d,\n", n->attr.regcl->bits ) ;
  fprintf( sf, "\t\t.regs = &jive_arch_regs[jive_arch_%s],\n", archi_regcl_regs_index_node_find(n)->attr.id ) ;
  fprintf( sf, "\t\t.nregs = %d,\n", n->attr.regcl->regs->attr.nregs ) ;
  fprintf( sf, "\t\t.index = jive_arch_%s,\n", n->attr.regcl->id ) ;
  if( n->attr.regcl->pregcl )
    fprintf( sf, "\t\t.parent = &jive_arch_regcls[jive_arch_%s],\n", n->attr.regcl->pregcl->attr.regcl->id ) ;
  else fprintf( sf, "\t\t.parent = 0,\n" ) ;
  fprintf( sf, "\t\t.class_mask = " ) ;
  archi_ast_node *l = n ;
  while( l ){
    fprintf( sf, "(1 << jive_arch_%s)", l->attr.regcl->id ) ;
    l = l->attr.regcl->pregcl ;
    if( l ) fprintf( sf, " | " ) ;
  }
  fprintf( sf, "\n\t},\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_regcl_code_generate_( c, sf ) ;
  }
}

static void archi_regcl_code_generate( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_cpureg_class jive_arch_regcls [] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_regcl_code_generate_( c, sf ) ;
  }

  fprintf( sf, "} ;\n\n" ) ;
}

void archi_regsect_generate( archi_symtab *st, archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( hf && sf && n && n->node_type == NT_REGSECT ) ;

  archi_reg_defs_generate( n, hf ) ;
  archi_regcl_defs_generate( n, hf ) ;
  archi_reg_code_generate( st, n, sf ) ;
  archi_regcl_code_generate( n, sf ) ;
}
