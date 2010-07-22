#include "symtab.h"
#include "debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ARCHI_ENTRY_CNT 211

typedef struct archi_symtab_entry_{
  struct archi_symtab_entry_ *next ;
  char *key ;
  archi_ast_node *node ;
} archi_symtab_entry ;

typedef struct archi_symtab_scope_{
  struct archi_symtab_scope_* prev_scope ;
  archi_symtab_entry* entry[ARCHI_ENTRY_CNT] ;
} archi_symtab_scope ;

static void archi_symtab_scope_init( archi_symtab_scope *sc, archi_symtab_scope *prev_scope )
{
  sc->prev_scope = prev_scope ;
  memset( sc->entry, 0, sizeof(archi_symtab_entry*)*ARCHI_ENTRY_CNT ) ;
}

void archi_symtab_init( archi_symtab *st )
{
  st->innermost_scope = NULL ;
  archi_symtab_push_scope( st ) ;
}

/*
void archi_symtab_destroy( archi_symtab *st )
{
  while( st->innermost_scope != NULL ){
    archi_symtab_pop_scope( st ) ;
  }
}
*/
void archi_symtab_push_scope( archi_symtab *st )
{
  archi_symtab_scope* sc = talloc( st, archi_symtab_scope ) ;
  archi_symtab_scope_init( sc, st->innermost_scope ) ;
  st->innermost_scope = sc ;
}

void archi_symtab_pop_scope( archi_symtab *st )
{
  archi_symtab_scope* sc = st->innermost_scope->prev_scope ;
  talloc_free( st->innermost_scope ) ;
  st->innermost_scope = sc ;  
}

/*void destroy_symtab( symtab t )
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
*/

#define SHIFT 4
static unsigned int hash( const char* key )
{
	unsigned int tmp = 0, i = 0 ;
	while( key[i] != '\0' ){
		tmp = ((tmp << SHIFT) + key[i]) % ARCHI_ENTRY_CNT ;
		i++ ; 
	}

	return tmp ;
}


archi_ast_node* archi_symtab_lookup( archi_symtab *st, const char* key )
{
  archi_symtab_scope* sc = st->innermost_scope ;
  for( ; sc != NULL; sc = sc->prev_scope ){
    archi_symtab_entry *e = sc->entry[hash(key)] ;
    for( ; e != NULL; e = e->next ){
      if( strcmp(key, e->key) == 0 ) return e->node ;
    }
  }

  return NULL ;
}

/*node* lookup( symtab t, const char* key )
{
	entry* e = t[hash(key)] ;

	for( ; e != NULL; e = e->next )
		if( strcmp(key, e->key) == 0 ) break ;

	if( e != NULL ) return e->n ;
	else return NULL ;
}
*/

void archi_symtab_insert( archi_symtab *st, const char* key, archi_ast_node *n )
{
  archi_symtab_entry* e = talloc( st->innermost_scope, archi_symtab_entry ) ;

  unsigned int h = hash(key) ;

  e->node = n ;
  e->key = talloc_strdup( e, key ) ;
  e->next = st->innermost_scope->entry[h] ;

  st->innermost_scope->entry[h] = e ;
}

/*
void insert( symtab t, const char* key, node* n )
{
	entry* e = malloc( sizeof(entry) ) ;
	
	unsigned int h = hash(key) ;

	e->n = n ;
	e->key = strdup( key ) ;
	e->next = t[h] ;

	t[h] = e ;
}
*/



void archi_symtab_print( archi_symtab *st )
{
  archi_symtab_scope *sc = st->innermost_scope ;
  while( sc ){
	  for( unsigned int i = 0; i < ARCHI_ENTRY_CNT; i++ ){
		  printf("%d:", i ) ;
		  archi_symtab_entry *e = sc->entry[i] ;
		  while( e != 0 ){
			  printf( "{ key: %s node: %p } ", e->key, e->node ) ;	
			  e = e->next ;
		  }
		  printf("\n") ;
	  }
    printf("\n") ;
    sc = sc->prev_scope ;
  }
}

static int archi_symtab_idlist_destroy( archi_symtab_idlist *l )
{
  while( l->next ) TALLOC_FREE( l->next ) ;

  TALLOC_FREE( l->id ) ;
  TALLOC_FREE( l ) ;

  return 0 ;
}

static archi_symtab_idlist* archi_symtab_idlist_create( TALLOC_CTX *ctx )
{
  archi_symtab_idlist *l = talloc( ctx, archi_symtab_idlist ) ;
  talloc_set_destructor( l, archi_symtab_idlist_destroy ) ;

  return l ;
}

archi_symtab_idlist* archi_symtab_idlist_add( archi_symtab_idlist *idl, char* cl[], uint32_t cnt )
{
  DEBUG_ASSERT( cl && cnt > 0 ) ;

  archi_symtab_idlist *tmp = idl ;
  for( uint32_t i = 0; i < cnt; i++ ){
    tmp = archi_symtab_idlist_create( idl ) ;
    tmp->id = talloc_strdup( tmp, cl[i] ) ; 
  }

  return tmp ;
} 

archi_symtab_idlist* archi_symtab_idlist_fill( archi_symtab *st, archi_ast_nodetype node_type )
{
  archi_symtab_idlist *idl  = NULL ;
  archi_symtab_scope *sc = st->innermost_scope ;
  while( sc ){
    for( uint32_t i = 0; i < ARCHI_ENTRY_CNT; i++ ){
      archi_symtab_entry *e = sc->entry[i] ;
      while( e != 0 ){
        if( e->node->node_type == node_type ){
          idl = archi_symtab_idlist_add( idl, (char* [1]){e->key}, 1 ) ; 
        }
        e = e->next ;
      }
    }
    sc = sc->prev_scope ;
  }

  return idl ;
}



