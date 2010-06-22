#ifndef ARCHI_ATTRIBUTES_H_
#define ARCHI_ATTRIBUTES_H_

#include <stdint.h>

#define ARCHI_REG_CODE_NOT_DEFINED -1
#define ARCHI_REGCL_BITS_NOT_DEFINED -1
#define ARCHI_REGCL_REGS_NOT_DEFINED NULL
#define ARCHI_REGCL_NREGS_NOT_DEFINED -1
#define ARCHI_REGCL_PREGCL_NOT_DEFINED NULL
#define ARCHI_NREGCLS_NOT_DEFINED -1 

struct archi_ast_node_ ;

typedef struct archi_regdef_attributes_{
  int32_t code ;
  const char *id ;
} archi_regdef_attributes ;

void archi_regdef_attributes_init( archi_regdef_attributes *attr ) ;

typedef struct archi_regcldef_attributes_{
  int32_t bits ;
  struct archi_ast_node_ *regs ;
  struct archi_ast_node_ *pregcl ; //parent register class
  const char *id ;
} archi_regcldef_attributes ;

void archi_regcldef_attributes_init( archi_regcldef_attributes *attr ) ;

typedef union node_attributes_{
  archi_regdef_attributes *reg ;
  archi_regcldef_attributes *regcl ;
  
  int32_t bits ;
  int32_t nregs ;
  int32_t nregcls ;
  const char* id ;
  int32_t code ;
} node_attributes ;

#endif
