#include "regsect.h"
#include "../debug.h"

static void archi_reg_defs_generate_( archi_ast_node *n, FILE *hf, uint32_t *i )
{
  archi_ast_node *c, *cc ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ) archi_reg_defs_generate_( c, hf, i );
    if( c->node_type == NT_REGS ){
      FOREACH_CHILD( c, cc ){
        fprintf( hf, "\tjive_arch_%s = %d,\n", cc->attr.nt_id.id, (*i)++ ) ;
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
  fprintf( hf, "\tjive_arch_%s = %d,\n", n->attr.nt_regcldef.id, (*i)++ ) ;
  
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

  FOREACH_CHILD( n->attr.nt_regcldef.regs, c ){
    archi_ast_node *l = archi_symtab_lookup( st, c->attr.nt_id.id ) ;
    fprintf( sf, "\t[jive_arch_%s] = {\n", l->attr.nt_regdef.id ) ;
    fprintf( sf, "\t\t.name = \"%s\",\n", l->attr.nt_regdef.id ) ;
    fprintf( sf, "\t\t.regcls = &jive_arch_regcls[jive_arch_%s],\n", l->attr.nt_regdef.regcl->attr.nt_regcldef.id ) ;
    fprintf( sf, "\t\t.code = %d,\n", l->attr.nt_regdef.code ) ;
    fprintf( sf, "\t\t.index = jive_arch_%s,\n", l->attr.nt_regdef.id ) ;
    fprintf( sf, "\t\t.class_mask = " ) ;
    archi_ast_node *r = l->attr.nt_regdef.regcl ;
    while( r ){
      fprintf( sf, "(1 << jive_arch_%s)", r->attr.nt_regcldef.id ) ;
      r = r->attr.nt_regcldef.pregcl ;
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

  return n->attr.nt_regcldef.regs->first_child ;
} 

static void archi_regcl_code_generate_( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "\t[jive_arch_%s] = {\n", n->attr.nt_regcldef.id ) ;
  fprintf( sf, "\t\t.name = \"%s\",\n", n->attr.nt_regcldef.id ) ;
  fprintf( sf, "\t\t.nbits = %d,\n", n->attr.nt_regcldef.bits ) ;
  fprintf( sf, "\t\t.regs = &jive_arch_regs[jive_arch_%s],\n", archi_regcl_regs_index_node_find(n)->attr.nt_id.id ) ;
  fprintf( sf, "\t\t.nregs = %d,\n", n->attr.nt_regcldef.regs->attr.nt_regs.nregs ) ;
  fprintf( sf, "\t\t.index = jive_arch_%s,\n", n->attr.nt_regcldef.id ) ;
  if( n->attr.nt_regcldef.pregcl )
    fprintf( sf, "\t\t.parent = &jive_arch_regcls[jive_arch_%s],\n", n->attr.nt_regcldef.pregcl->attr.nt_regcldef.id ) ;
  else fprintf( sf, "\t\t.parent = 0,\n" ) ;
  fprintf( sf, "\t\t.class_mask = " ) ;
  archi_ast_node *l = n ;
  while( l ){
    fprintf( sf, "(1 << jive_arch_%s)", l->attr.nt_regcldef.id ) ;
    l = l->attr.nt_regcldef.pregcl ;
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
  DEBUG_ASSERT( hf && sf && st && n && n->node_type == NT_REGSECT ) ;

  archi_reg_defs_generate( n, hf ) ;
  archi_regcl_defs_generate( n, hf ) ;
  archi_reg_code_generate( st, n, sf ) ;
  archi_regcl_code_generate( n, sf ) ;
}
