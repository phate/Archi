#include "regsect.h"
#include "../debug.h"
/*
static void archi_regid_generate( const char *id, FILE *hf )
{
  fprintf( hf, "jive_arch_%s", id ) ;
}
*/

static void archi_reg_defs_generate( archi_ast_node *n, FILE *hf )
{
  fprintf( hf, "typedef enum {\n" ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGDEF )
      fprintf( hf, "\tjive_arch_%s = %d,\n", c->attr.reg->id, i++ ) ;
  }

  fprintf( hf, "} jive_arch_reg_index ;\n\n" ) ;
}

static void archi_regcl_defs_generate( archi_ast_node *n, FILE *hf )
{
  fprintf( hf, "typedef enum {\n" ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF )
      fprintf( hf, "\tjive_arch_%s = %d,\n", c->attr.regcl->id, i++ ) ;
  }

  fprintf( hf, "} jive_arch_regcls_index ;\n\n" ) ;

}

static void archi_reg_code_generate( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_cpureg jive_arch_regs [] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGDEF ){
      fprintf( sf, "\t[jive_arch_%s] = {\n", c->attr.reg->id ) ;
      fprintf( sf, "\t\t.name = \"%s\",\n", c->attr.reg->id ) ;
      fprintf( sf, "\t\t.regcls = &jive_arch_regcls[jive_arch_%s],\n", c->attr.reg->regcl->attr.regcl->id ) ;
      fprintf( sf, "\t\t.code = %d,\n", c->attr.reg->code ) ;
      fprintf( sf, "\t\t.index = jive_arch_%s,\n", c->attr.reg->id ) ;
      fprintf( sf, "\t\t.class_mask = " ) ;
      archi_ast_node *l = c->attr.reg->regcl ;
      while( l ){
        fprintf( sf, "(1 << jive_arch_%s)", l->attr.regcl->id ) ;
        l = l->attr.regcl->pregcl ;
        if( l ) fprintf( sf, " | " ) ;
      }
      fprintf( sf, "\n\t},\n" ) ;
    }
  }

  fprintf( sf, "} ;\n\n" ) ;
}

static void archi_regcl_code_generate( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_cpureg_class jive_arch_regcls [] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_REGCLDEF ){
      fprintf( sf, "\t[jive_arch_%s] = {\n", c->attr.regcl->id ) ;
      fprintf( sf, "\t\t.name = \"%s\",\n", c->attr.regcl->id ) ;
      fprintf( sf, "\t\t.nbits = %d,\n", c->attr.regcl->bits ) ;
      fprintf( sf, "\t\t.regs = &(jive_cpureg[]){ " ) ;
      archi_ast_node *l ;
      FOREACH_CHILD( c->attr.regcl->regs, l ){
        fprintf( sf, "jive_arch_regs[jive_arch_%s], ", l->attr.id ) ; 
      }
      fprintf( sf, " },\n") ;
      fprintf( sf, "\t\t.nregs = %d,\n", c->attr.regcl->regs->attr.nregs ) ;
      fprintf( sf, "\t\t.index = jive_arch_%s,\n", c->attr.regcl->id ) ;
      if( c->attr.regcl->pregcl )
        fprintf( sf, "\t\t.parent = &jive_arch_regcls[jive_arch_%s],\n", c->attr.regcl->pregcl->attr.regcl->id ) ;
      else fprintf( sf, "\t\t.parent = 0,\n" ) ;
      fprintf( sf, "\t\t.class_mask = " ) ;
      l = c ;
      while( l ){
        fprintf( sf, "(1 << jive_arch_%s)", l->attr.regcl->id ) ;
        l = l->attr.regcl->pregcl ;
        if( l ) fprintf( sf, " | " ) ;
      }
      fprintf( sf, "\n\t},\n" ) ;
    }
  }

  fprintf( sf, "} ;\n\n" ) ;
}

void archi_regsect_generate( archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( hf && sf && n && n->node_type == NT_REGSECT ) ;

  archi_reg_defs_generate( n, hf ) ;
  archi_regcl_defs_generate( n, hf ) ;
  archi_reg_code_generate( n, sf ) ;
  archi_regcl_code_generate( n, sf ) ;
}
