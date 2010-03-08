#ifndef EHANDLING_H_
#define EHANDLING_H_

#include <stdint.h>

struct node_ ;

typedef struct emsg_ emsg ;

uint32_t print_msgs( struct node_ *n ) ;

void add_emsg( struct node_ *n, const char *msg, ... ) ;
#endif
