#ifndef ARCHI_ATTRIBUTES_H_
#define ARCHI_ATTRIBUTES_H_

#include <stdint.h>

struct archi_ast_node_ ;

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

typedef struct archi_nt_id_attributes_{
  const char* id ;
} archi_nt_id_attributes ;

typedef struct archi_nt_tid_attributes_{
  const char* id ;
} archi_nt_tid_attributes ;

typedef union node_attributes_{
  archi_nt_regdef_attributes nt_regdef ;
  archi_nt_regcldef_attributes nt_regcldef ;
  archi_nt_bits_attributes nt_bits ;
  archi_nt_code_attributes nt_code ;
  archi_nt_regs_attributes nt_regs ;
  archi_nt_regsect_attributes nt_regsect ;
  archi_nt_id_attributes nt_id ;
  archi_nt_tid_attributes nt_tid ; 

/*  int32_t bits ;
  int32_t nregs ;
  int32_t nregcls ;
  const char* id ;
  int32_t code ;
*/
} node_attributes ;

#endif
