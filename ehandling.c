#include "ehandling.h"
#include "nodes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

struct archi_emsg_{
	struct archi_emsg_* next ;
	char* msg ;
} ;

void archi_destroy_emsgs( archi_emsg *emsg )
{
  if( emsg == NULL ) return ;
  free( emsg->msg ) ;
  archi_destroy_emsgs( emsg->next ) ;
  free( emsg ) ;
}

static archi_emsg* create_msg( const char* msg, uint32_t linenr )
{
	//TODO: hard coded, fix it
	char tmp[256] ;
	int cnt = snprintf( tmp, 256, "%d:\terror: %s", linenr, msg ) ;
	if( cnt >= 256 ) assert(0) ;

	archi_emsg* m = (archi_emsg*)malloc( sizeof(archi_emsg) ) ;
	
	m->msg = strdup( tmp ) ;
	m->next = NULL ;

	return m ;
}

static archi_emsg* append_msgs( archi_emsg* m1, archi_emsg* m2 )
{
	if( m1 == NULL ) return m2 ;
	if( m2 == NULL ) return m1 ;

	archi_emsg* tmp = m1 ;
	while( m1->next != NULL ){ m1 = m1->next ; }

	m1->next = m2 ;

	return tmp ;	
}

void archi_add_emsg( archi_ast_node *n, const char *msg, ... )
{
	//TODO: hard coded, fix it
	char tmp[256] ;
	
	va_list args ;
  va_start( args, msg ) ; 
  int cnt = vsnprintf( tmp, 256, msg, args ) ;   
  va_end(args) ;

  if( cnt >= 256 ) assert(0) ;

	archi_emsg *m = create_msg( tmp, n->linenr ) ;
	n->emsg_list = append_msgs( n->emsg_list, m ) ;
}

static archi_emsg* linearize_msgs( archi_ast_node *n )
{
	archi_ast_node* c ;
	FOREACH_CHILD( n, c ){
		archi_emsg *msgs = linearize_msgs( c ) ;
		n->emsg_list = append_msgs( n->emsg_list, msgs ) ;
	}

	return n->emsg_list ;	
}

uint32_t archi_print_emsgs( archi_ast_node *n )
{
	archi_emsg *msgs = linearize_msgs( n ) ;

	uint32_t cnt = 0 ;
	while( msgs ){
		printf("%s\n", msgs->msg) ; 

		cnt++ ;
		msgs = msgs->next ;
	}

	return cnt ;
}
