#ifndef ARCHI_EHANDLING_H_
#define ARCHI_EHANDLING_H_

#include <stdint.h>

#define EMSG_MULTIPLE_ATTRIBUTE( node, attr ) \
  archi_add_emsg( node, "attribute '%s' is declared more than once", attr )

struct archi_ast_node_ ;

typedef struct archi_emsg_ archi_emsg ;

uint32_t archi_print_emsgs( struct archi_ast_node_ *n ) ;

void archi_add_emsg( struct archi_ast_node_ *n, const char *msg, ... ) ;
void archi_destroy_emsgs( archi_emsg *emsg ) ;

#endif
