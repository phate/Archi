#include "instrsect.h"
#include "../debug.h"

static void archi_instr_defs_generate( archi_ast_node *n, FILE *hf )
{
  fprintf( hf, "typedef enum {\n" ) ;

  uint32_t i = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_INSTRDEF ) 
      fprintf( hf, "\tjive_arch_%s = %d,\n", c->attr.nt_instrdef.id, i++ ) ;
  }

  fprintf( hf, "} jive_arch_instruction_index ;\n\n" ) ;
}

static void archi_instr_code_generate( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_instruction_class jive_arch_instructions[] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    fprintf( sf, "\t[jive_arch_%s] = {\n", c->attr.nt_instrdef.id ) ;
    fprintf( sf, "\t\t.name = \"%s\",\n", c->attr.nt_instrdef.id ) ;
    fprintf( sf, "\t\t.encode = 0,\n" ) ;
    fprintf( sf, "\t\t.mnemonic = 0,\n" ) ;
    fprintf( sf, "\t\t.inregs = 0,\n" ) ;
    fprintf( sf, "\t\t.outregs = 0,\n" ) ;
    fprintf( sf, "\t\t.flags = jive_instruction_flags_none,\n" ) ;
    fprintf( sf, "\t\t.ninputs = %d,\n", c->attr.nt_instrdef.input->attr.nt_input.nregs ) ;
    fprintf( sf, "\t\t.noutputs = %d,\n", c->attr.nt_instrdef.output->attr.nt_output.nregs ) ;
    fprintf( sf, "\t\t.nimmediates = %d,\n", c->attr.nt_instrdef.input->attr.nt_input.nints ) ;
    fprintf( sf, "\t\t.code = 0\n" ) ;
    fprintf( sf, "\t},\n" ) ;
  }

  fprintf( sf, "} ;\n\n" ) ;
}

void archi_instrsect_generate( archi_ast_node *n, FILE *hf, FILE *sf )
{
  DEBUG_ASSERT( hf && sf && n && n->node_type == NT_INSTRSECT ) ;

  archi_instr_defs_generate( n, hf ) ;
  archi_instr_code_generate( n, sf ) ; 
}
