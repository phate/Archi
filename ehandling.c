#include "ehandling.h"
#include "nodes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct emsg_{
	struct emsg_* next ;
	const char* msg ;
} ;


emsg* create_msg( uint32_t linenr, const char* msg )
{
	char tmp[256] ;
	snprintf( tmp, 256, "%d:\terror: %s", linenr, msg ) ;

	emsg* m = (emsg*)malloc( sizeof(emsg) ) ;
	
	m->msg = strdup( tmp ) ;
	m->next = NULL ;

	return m ;
}

emsg* append_msgs( emsg* m1, emsg* m2 )
{
	if( m1 == NULL ) return m2 ;
	if( m2 == NULL ) return m1 ;

	emsg* tmp = m1 ;
	while( m1->next != NULL ){ m1 = m1->next ; }

	m1->next = m2 ;

	return tmp ;	
}

static emsg* linearize_msgs( node* n )
{
	node* c ;
	FOREACH_CHILD( n, c ){
		emsg *msgs = linearize_msgs( c ) ;
		n->emsgs = append_msgs( n->emsgs, msgs ) ;
	}

	return n->emsgs ;	
}

uint32_t print_msgs( node* n )
{
	emsg *msgs = linearize_msgs( n ) ;

	uint32_t cnt = 0 ;
	while( msgs ){
		printf("%s\n", msgs->msg) ; 

		cnt++ ;
		msgs = msgs->next ;
	}

	return cnt ;
}
