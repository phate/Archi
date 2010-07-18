#ifndef ARCHI_ATTRIBUTES_H_
#define ARCHI_ATTRIBUTES_H_

#include <stdint.h>

struct archi_ast_node_ ;

#define ARCHI_INSTR_FLAGS \
  X( NT_OVERWRITEINPUTFLAG, write_input, 1 ) \
  X( NT_COMMUTATIVEFLAG, commutative, 2 )

typedef struct archi_nt_regdef_attributes_{
  int32_t code ;
  struct archi_ast_node_ *regcl ;
  const char *id ;
} archi_nt_regdef_attributes ;

void archi_nt_regdef_attributes_init( archi_nt_regdef_attributes *attr ) ;

typedef struct archi_nt_regcldef_attributes_{
  int32_t bits ;
  struct archi_ast_node_ *regs ;
  struct archi_ast_node_ *pregcl ; //parent register class
  const char *id ;
} archi_nt_regcldef_attributes ;

void archi_nt_regcldef_attributes_init( archi_nt_regcldef_attributes *attr ) ;

typedef struct archi_nt_instrdef_attributes_{
  const char *id ;
  struct archi_ast_node_ *input ;
  struct archi_ast_node_ *output ;
  struct archi_ast_node_ *encoding ;
  struct archi_ast_node_ *flags ;
} archi_nt_instrdef_attributes ;

void archi_nt_instrdef_attributes_init( archi_nt_instrdef_attributes *attr ) ;

typedef struct archi_nt_matchdef_attributes_{
  const char* id ;
  struct archi_ast_node_ *input ;
  struct archi_ast_node_ *output ;
} archi_nt_matchdef_attributes ;

void archi_nt_matchdef_attributes_init( archi_nt_matchdef_attributes *attr ) ;

typedef struct archi_nt_jvinstrdef_attributes_{
  const char* id ;
  int32_t ninputs ;
  int32_t noutputs ;
} archi_nt_jvinstrdef_attributes ;

typedef struct archi_nt_input_attributes_{
  int32_t nints ;
  int32_t nregs ;
} archi_nt_input_attributes ;

void archi_nt_input_attributes_init( archi_nt_input_attributes *attr ) ;

typedef struct archi_nt_flags_attributes_{
  int32_t flags ;
} archi_nt_flags_attributes ;

typedef struct archi_nt_bstr_attributes_{
  const char* bstr ;
  int32_t len ;
} archi_nt_bstr_attributes ;

void archi_nt_bstr_attributes_init( archi_nt_bstr_attributes *attr ) ;

typedef struct archi_nt_bslc_attributes_{
  int32_t start ;
  int32_t length ;
} archi_nt_bslc_attributes ;

void archi_nt_bslc_attributes_init( archi_nt_bslc_attributes *attr ) ;

typedef struct archi_nt_bits_attributes_{
  int32_t bits ;
} archi_nt_bits_attributes ;

typedef struct archi_nt_code_attributes_{
  int32_t code ;
} archi_nt_code_attributes ;

typedef struct archi_nt_regs_attributes_{
  int32_t nregs ;
} archi_nt_regs_attributes ;

typedef struct archi_nt_regsect_attributes_{
  int32_t nregcls ;
} archi_nt_regsect_attributes ;

typedef struct archi_nt_output_attributes_{
  int32_t nregs ;
} archi_nt_output_attributes ;

typedef struct archi_nt_id_attributes_{
  const char* id ;
} archi_nt_id_attributes ;

typedef struct archi_nt_tid_attributes_{
  const char* id ;
} archi_nt_tid_attributes ;

typedef struct archi_nt_concat_attributes_{
  int32_t len ;
} archi_nt_concat_attributes ;

typedef struct archi_nt_num_attributes_{
  int32_t num ;
} archi_nt_num_attributes ;


typedef union node_attributes_{
  archi_nt_regdef_attributes nt_regdef ;
  archi_nt_regcldef_attributes nt_regcldef ;
  archi_nt_instrdef_attributes nt_instrdef ;
  archi_nt_jvinstrdef_attributes nt_jvinstrdef ;
  archi_nt_matchdef_attributes nt_matchdef ;
  archi_nt_bits_attributes nt_bits ;
  archi_nt_code_attributes nt_code ;
  archi_nt_regs_attributes nt_regs ;
  archi_nt_flags_attributes nt_flags ;
  archi_nt_regsect_attributes nt_regsect ;
  archi_nt_input_attributes nt_input ;
  archi_nt_output_attributes nt_output ;
  archi_nt_concat_attributes nt_concat ;
  archi_nt_bslc_attributes nt_bslc ;
  archi_nt_bstr_attributes nt_bstr ;
  archi_nt_num_attributes nt_num ;
  archi_nt_id_attributes nt_id ;
  archi_nt_tid_attributes nt_tid ; 

} node_attributes ;

#endif
