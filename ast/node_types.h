#ifndef ARCHI_NODE_TYPES_H_
#define ARCHI_NODE_TYPES_H_

#define ARCHI_AST_NODETYPE \
  X( NT_ARCHDEF ) \
  X( NT_REGSECT ) \
  X( NT_INSTRSECT ) \
  X( NT_PATTERNSECT ) \
  X( NT_REGDEF ) \
  X( NT_CODE ) \
  X( NT_REGCLDEF ) \
  X( NT_REGS ) \
  X( NT_BITS ) \
  X( NT_INSTRDEF ) \
  X( NT_INPUT ) \
  X( NT_OUTPUT ) \
  X( NT_ENCODING ) \
  X( NT_FLAGS ) \
  X( NT_OVERWRITEINPUTFLAG ) \
  X( NT_COMMUTATIVEFLAG ) \
  X( NT_ANODEDEF ) \
  X( NT_MATCHES ) \
  X( NT_MATCHDEF ) \
  X( NT_OPATTERN ) \
  X( NT_IPATTERN ) \
  X( NT_REFNODE ) \
  X( NT_NODEDEF ) \
  X( NT_JVINSTRDEF ) \
  X( NT_IFTHENELSE ) \
  X( NT_EQUAL ) \
  X( NT_BSLC ) \
  X( NT_DOT ) \
  X( NT_CONCAT ) \
  X( NT_BSTR ) \
  X( NT_STR ) \
  X( NT_NUM ) \
  X( NT_TRUE ) \
  X( NT_FALSE ) \
  X( NT_ID ) \
  X( NT_TID ) \

//Name, nInpute, nOutput, Jive Node Class
#define JIVE_PREDEF_INSTR \
  X( JVBitconstant, 0, 1, JIVE_REGCONSTANT ) \
  X( JVAdd, 2, 1, JIVE_FIXEDADD ) \
  X( JVLoad, 1, 1, JIVE_LOAD ) \
  X( JVStore, 2, 0, JIVE_STORE )

#endif
