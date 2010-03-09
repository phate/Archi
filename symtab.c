#include "symtab.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

symtab create_symtab()
{
	symtab t = malloc( SIZE*sizeof(entry*) ) ;
	memset( t, 0, SIZE*sizeof(entry*) ) ;
	
	return t ;
}

void destroy_symtab( symtab t )
{
	for( unsigned int i = 0; i < SIZE; i++ ){
		entry* e = t[i] ;
		while( t[i] != 0 ){
			t[i] = e->next ;
			free( e->key ) ;
			free( e ) ;
		}
	}

	free( t ) ;
}

#define SHIFT 4
static unsigned int hash( const char* key )
{
	unsigned int tmp = 0, i = 0 ;
	while( key[i] != '\0' ){
		tmp = ((tmp << SHIFT) + key[i]) % SIZE ;
		i++ ; 
	}

	return tmp ;
}

node* lookup( symtab t, const char* key )
{
	entry* e = t[hash(key)] ;

	for( ; e != NULL; e = e->next )
		if( strcmp(key, e->key) == 0 ) break ;

	if( e != NULL ) return e->n ;
	else return NULL ;
}

void insert( symtab t, const char* key, node* n )
{
	entry* e = malloc( sizeof(entry) ) ;
	
	unsigned int h = hash(key) ;

	e->n = n ;
	e->key = strdup( key ) ;
	e->next = t[h] ;

	t[h] = e ;
}

void print_symtab( symtab t )
{
	for( unsigned int i = 0; i < SIZE; i++ ){
		printf("%d:", i ) ;
		entry* e = t[i] ;
		while( e != 0 ){
			printf( "{ key: %s node: %p } ", e->key, e->n ) ;	
			e = e->next ;
		}
		printf("\n") ;
	}
}
