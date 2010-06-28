#ifndef ARCHI_EHANDLING_H_
#define ARCHI_EHANDLING_H_

#include <stdint.h>

#define EMSG_MULTIPLE_ATTRIBUTE( node, attr ) \
  archi_add_emsg( node, "attribute '%s' is declared more than once", attr )

#define EMSG_MISSING_ATTRIBUTE( node, attr ) \
  archi_add_emsg( node, "attribute '%s' was not defined", attr )

#define EMSG_MISSING_REGISTER( node, reg ) \
  archi_add_emsg( node, "register '%s' was not defined", reg )

#define EMSG_WRONG_TYPE( node, id, type ) \
  archi_add_emsg( node, "'%s' is not of type '%s'", id, type )

#define EMSG_REGCLS_NOT_HIERARCHICAL( regcl0, regcl1 ) \
  archi_add_emsg( regcl0, "register classes '%s' and '%s' are not hierarchical", \
  regcl0->attr.regcl->id, regcl1->attr.regcl->id )

#define EMSG_REGISTER_NOT_USED( node, id ) \
  archi_add_emsg( node, "register '%s' was not used in a register class", id )

struct archi_ast_node_ ;

typedef struct archi_emsg_ archi_emsg ;

uint32_t archi_print_emsgs( struct archi_ast_node_ *n ) ;

void archi_add_emsg( struct archi_ast_node_ *n, const char *msg, ... ) ;
void archi_destroy_emsgs( archi_emsg *emsg ) ;

#endif
