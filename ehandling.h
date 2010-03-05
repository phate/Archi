#ifndef EHANDLING_H_
#define EHANDLING_H_

#include <stdint.h>

struct node_ ;

typedef struct emsg_ emsg ;

emsg* append_msgs( emsg* m1, emsg* m2 ) ;

emsg* create_msg( uint32_t linenr, const char* msg ) ;

uint32_t print_msgs( struct node_ *n ) ;

#endif
