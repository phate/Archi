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

static int32_t archi_instr_encoding_generate_( archi_ast_node *n, FILE *sf, int32_t blen )
{
  switch( n->node_type ){
    case NT_BSTR:{
      int32_t bstrlen = n->attr.nt_bstr.len ;

      int32_t p = 0 ;
      while( p < n->attr.nt_bstr.len ){
        int32_t bmissing = 8 - blen ;
        if( bmissing == 8 ) fprintf( sf, "\tjive_buffer_putbyte( target, " ) ;

        if( bstrlen < bmissing ){
          fprintf( sf, "0x%lX << %d | ", strtol(n->attr.nt_bstr.bstr+p, 0, 2), 8-bstrlen-blen ) ;
          return blen+bstrlen ; 
        }
        if( bstrlen == bmissing ){
          fprintf( sf, "0x%lX ) ;\n", strtol(n->attr.nt_bstr.bstr+p, 0, 2) ) ;
          return 0 ;
        }
        if( bstrlen > bmissing ){
          const char *bstr = talloc_strndup( n, n->attr.nt_bstr.bstr+p, bmissing ) ;
          fprintf( sf, "0x%lX ) ;\n", strtol(bstr, 0, 2) ) ;
          p += bmissing ;
          bstrlen -= bmissing ;
          blen = 0 ;
        }
      }
    break ;}
    case NT_CONCAT:{
      int32_t l = archi_instr_encoding_generate_( n->first_child, sf, blen ) ; 
      return archi_instr_encoding_generate_( n->last_child, sf, l ) ;
    break ;}
    default: DEBUG_ASSERT(0) ; return 0 ;
  }

  return 0 ;
}

static void archi_instr_encoding_generate( archi_ast_node *n, FILE *sf )
{
  DEBUG_ASSERT( sf && n && n->node_type == NT_INSTRDEF ) ;

  fprintf( sf, "jive_encode_result jive_arch_%s_generate(\n", n->attr.nt_instrdef.id ) ;
  fprintf( sf, "\tjive_buffer *target,\n" ) ;
  fprintf( sf, "\tconst jive_instruction_class *icls,\n" ) ;
  fprintf( sf, "\tconst jive_cpureg *inputs[],\n" ) ;
  fprintf( sf, "\tconst jive_cpureg *outputs[],\n" ) ;
  fprintf( sf, "\tconst long immediates[])\n{\n" ) ;

  archi_instr_encoding_generate_( n->attr.nt_instrdef.encoding->first_child, sf, 0 ) ;

  fprintf( sf, "}\n\n" ) ;
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

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ) archi_instr_encoding_generate( c, sf ) ; 
}
