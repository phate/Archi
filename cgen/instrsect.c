#include "instrsect.h"
#include "../debug.h"

#include <math.h>
#include <string.h>

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

static void archi_simple_print( archi_ast_node *n, FILE *sf )
{
  switch( n->node_type ){
    case NT_TRUE:
      fprintf( sf, "1" ) ; break ;
    case NT_FALSE:
      fprintf( sf, "0" ) ; break ; 
    case NT_NUM:
      fprintf( sf, "%d", n->attr.nt_num.num ) ; break ;
    case NT_BSTR:
      fprintf( sf, "%lX", strtol(n->attr.nt_bstr.bstr, 0, 2) ) ; break ;
    case NT_ID:
      fprintf( sf, "%s", n->attr.nt_id.id ) ; break ;
    default: DEBUG_ASSERT(0) ;
  }
}

static int32_t archi_instr_encoding_generate_( archi_ast_node *n, FILE *sf, int32_t blen )
{
  switch( n->node_type ){
    //assumes that the length of each case is a multiple of eight and that before if is a multiple of eight!!!
    case NT_IFTHENELSE:{
      fprintf( sf, "\tif( " ) ;
      archi_simple_print( n->attr.nt_ifthenelse.pred, sf ) ;
      fprintf( sf, " ){\n" ) ;
      archi_instr_encoding_generate_( n->attr.nt_ifthenelse.cthen, sf, blen ) ;
      fprintf( sf, "\t}else{\n" ) ;
      archi_instr_encoding_generate_( n->attr.nt_ifthenelse.celse, sf, blen ) ;
      fprintf( sf, "\t}\n\n" ) ;
    break ;}
    case NT_BSTR:{
      int32_t bstrlen = n->attr.nt_bstr.len ;

      int32_t p = 0 ;
      while( p < n->attr.nt_bstr.len ){
        int32_t bmissing = 8 - blen ;
        if( bmissing == 8 ) fprintf( sf, "\tif( !jive_buffer_putbyte( target, " ) ;

        if( bstrlen < bmissing ){
          fprintf( sf, "0x%lX << %d | ", strtol(n->attr.nt_bstr.bstr+p, 0, 2), 8-bstrlen-blen ) ;
          return blen+bstrlen ; 
        }
        if( bstrlen == bmissing ){
          fprintf( sf, "0x%lX ) )\n\t\treturn jive_encode_out_of_memory ;\n", strtol(n->attr.nt_bstr.bstr+p, 0, 2) ) ;
          return 0 ;
        }
        if( bstrlen > bmissing ){
          const char *bstr = talloc_strndup( n, n->attr.nt_bstr.bstr+p, bmissing ) ;
          fprintf( sf, "0x%lX ) )\n\t\treturn jive_encode_out_of_memory ;\n", strtol(bstr, 0, 2) ) ;
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
    case NT_BSLC:{
      int32_t bslclen = n->attr.nt_bslc.length ;
    
      int32_t p = 0 ;
      while( p < n->attr.nt_bslc.length ){
        int32_t bmissing = 8 - blen ;
        if( bmissing == 8 ) fprintf( sf, "\tif( !jive_buffer_putbyte( target, " ) ;

        if( bslclen < bmissing ){
          fprintf( sf, "(((" ) ;
          archi_simple_print( n->first_child, sf ) ;
          fprintf( sf, ") >> %d) & 0x%lX) << %d | ", n->attr.nt_bslc.start, (long)pow(2, bslclen)-1, bmissing-bslclen ) ;
          return blen+bslclen ;
        }
        if( bslclen == bmissing ){
          fprintf( sf, "(((" ) ;
          archi_simple_print( n->first_child, sf ) ;
          fprintf( sf, ") >> %d) & 0x%lX) )\n\t\treturn jive_encode_out_of_memory ;\n", n->attr.nt_bslc.start, (long)pow(2, bslclen)-1 ) ;
          return 0 ; 
        }
        if( bslclen > bmissing ){
          fprintf( sf, "(((" ) ;
          archi_simple_print( n->first_child, sf ) ;
          fprintf( sf, ") >> %d) & 0x%lX) )\n\t\treturn jive_encode_out_of_memory ;\n", n->attr.nt_bslc.start+bslclen-bmissing, (long)pow(2, bmissing)-1 ) ;
          p += bmissing ;
          bslclen -= bmissing ;
          blen = 0 ;
        }
      }
    break ;}
    default: DEBUG_ASSERT(0) ; return 0 ;
  }

  return blen ;
}

static void archi_instr_io_generate( archi_ast_node *n, FILE *sf )
{
  DEBUG_ASSERT( n && (n->node_type == NT_INPUT || n->node_type == NT_OUTPUT) ) ;

  int32_t nints = 0 ;
  int32_t nregs = 0 ;
  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( n->node_type == NT_INPUT ){
      if( !strcmp(c->data_type, "Int") )
        fprintf( sf, "\tuint32_t %s = cpu_to_le32(immediates[%d]) ;\n", c->attr.nt_tid.id, nints++ ) ;
      else
        fprintf( sf, "\tint %s = inputs[%d]->code ;\n", c->attr.nt_tid.id, nregs++ ) ;
    }
    else{
      fprintf( sf, "\tint %s = outputs[%d]->code ;\n", c->attr.nt_tid.id, nregs++ ) ;
    }
  }

  fprintf(sf, "\n" ) ;
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

  archi_instr_io_generate( n->attr.nt_instrdef.input, sf ) ;
  archi_instr_io_generate( n->attr.nt_instrdef.output, sf ) ;
  archi_instr_encoding_generate_( n->attr.nt_instrdef.encoding->first_child, sf, 0 ) ;

  fprintf( sf, "\n\treturn jive_encode_ok ;\n}\n\n" ) ;
}

static void archi_instr_flags_generate( archi_ast_node *n, FILE *sf )
{
  if( !n ){
    fprintf( sf, "jive_instruction_flags_none,\n" ) ;
    return ;
  }
  DEBUG_ASSERT( n->node_type == NT_FLAGS ) ;

  
  #define X( nt, op, p ) \
    if( n->attr.nt_flags.flags & p ) \
      fprintf( sf, "jive_instruction_flags_%s | ", #op ) ;

  ARCHI_INSTR_FLAGS
  #undef X

  fprintf( sf, " jive_instruction_flags_none,\n" ) ;
}

static void archi_instr_code_generate( archi_ast_node *n, FILE *sf )
{
  fprintf( sf, "const jive_instruction_class jive_arch_instructions[] = {\n" ) ;

  archi_ast_node *c ;
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_JVINSTRDEF ) continue ;
    fprintf( sf, "\t[jive_arch_%s] = {\n", c->attr.nt_instrdef.id ) ;
    fprintf( sf, "\t\t.name = \"%s\",\n", c->attr.nt_instrdef.id ) ;
    fprintf( sf, "\t\t.encode = &jive_arch_%s_generate,\n", c->attr.nt_instrdef.id ) ;
    fprintf( sf, "\t\t.mnemonic = 0,\n" ) ;
    fprintf( sf, "\t\t.inregs = 0,\n" ) ;
    fprintf( sf, "\t\t.outregs = 0,\n" ) ;
    fprintf( sf, "\t\t.flags = " ) ;
    archi_instr_flags_generate( c->attr.nt_instrdef.flags, sf ) ;
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
  FOREACH_CHILD( n, c ){
    if( c->node_type == NT_JVINSTRDEF ) continue ;
    archi_instr_encoding_generate( c, sf ) ;
  } 
}
