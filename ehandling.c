#include "ehandling.h"
#include "nodes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

struct emsg_{
	struct emsg_* next ;
	const char* msg ;
} ;


static emsg* create_msg( const char* msg, uint32_t linenr )
{
	char tmp[256] ;
	int cnt = snprintf( tmp, 256, "%d:\terror: %s", linenr, msg ) ;
	if( cnt >= 256 ) assert(0) ;

	emsg* m = (emsg*)malloc( sizeof(emsg) ) ;
	
	m->msg = strdup( tmp ) ;
	m->next = NULL ;

	return m ;
}

static emsg* append_msgs( emsg* m1, emsg* m2 )
{
	if( m1 == NULL ) return m2 ;
	if( m2 == NULL ) return m1 ;

	emsg* tmp = m1 ;
	while( m1->next != NULL ){ m1 = m1->next ; }

	m1->next = m2 ;

	return tmp ;	
}

void add_emsg( node *n, const char *msg, ... )
{
	char tmp[256] ;
	
	va_list args ;
  va_start( args, msg ) ; 
  int cnt = vsnprintf( tmp, 256, msg, args ) ;   
  va_end(args) ;

  if( cnt >= 256 ) assert(0) ;

	emsg *m = create_msg( tmp, n->linenr ) ;
	n->emsgs = append_msgs( n->emsgs, m ) ;
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
