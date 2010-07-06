#include "parser.h"
#include "ast/node.h"
#include "symtab.h"
#include "tc/typecheck.h"
#include "cgen/cgen.h"
#include "ehandling.h"

#include <stdio.h>
#include <stdlib.h>

extern archi_ast_node* archi_parse() ;
extern FILE *yyin ;

FILE *ad = NULL ;
FILE *hf = NULL ;
FILE *sf = NULL ;
archi_symtab *symtab = NULL ;
archi_ast_node *ast = NULL ;

static void archi_cleanup()
{
  if( ad != NULL ) fclose( ad ) ;
	if( hf != NULL ) fclose( hf ) ;
	if( sf != NULL ) fclose( sf ) ;

  TALLOC_FREE( ast ) ;
  TALLOC_FREE( symtab ) ;
}

static FILE* archi_file_open( const char *filename, const char *mode )
{
	FILE *f = fopen( filename, mode ) ;
	if( f == NULL ){
		fprintf( stderr, "Cannot open file: %s\n", filename ) ;
		archi_cleanup() ;
		exit( EXIT_FAILURE ) ; 
	}

	return f ;
}

static void process_input( int argc, char* argv[] )
{
/*
  if( argc != 4 ){
		fprintf( stderr, "Invalid number of arguments:\n" ) ;
		fprintf( stderr, "%s <arch description> <output header file> <output source file>\n", argv[0] ) ;
		exit( EXIT_FAILURE ) ;
	}
*/
	ad = archi_file_open( "arch/ppc.ca"/*argv[1]*/, "r" ) ;
//	hf = open_file( argv[2], "w" ) ;
//	sf = open_file( argv[3], "w" ) ;	
} 

static void archi_init( int argc, char* argv[] ){
	process_input( argc, argv ) ;
	yyin = ad ;

  symtab = talloc( NULL, archi_symtab ) ;
  archi_symtab_init( symtab ) ; 
}

static void report_errors( archi_ast_node *n )
{
	uint32_t cnt = archi_print_emsgs( n ) ;
	if( cnt != 0 ){
		archi_cleanup() ;
	 	exit( EXIT_FAILURE ) ;
	}
}

int main( int argc, char* argv[] )
{
  archi_init( argc, argv ) ;
	
	ast = archi_parse() ;
	if( ast == NULL ){
		archi_cleanup() ;
		exit( EXIT_FAILURE ) ;
	}
	archi_view_ast(ast) ;

  archi_typecheck( symtab, ast ) ;
 // archi_view_ast( ast ) ;
//  archi_symtab_print( symtab ) ;  
  report_errors( ast ) ;

  archi_code_generate( symtab, ast, stdout, stdout ) ;

/*	
  trim_tree( ast ) ;
	fill_symtab( stab, ast ) ;
	report_errors( ast ) ;
	
	print_symtab( stab ) ;
	view_tree( ast ) ;
	typecheck( stab, ast ) ;
	report_errors( ast ) ;

  generate_code( ast, stdout, stdout ) ;
*/

//  talloc_report_full( ast, stdout ) ;

	archi_cleanup() ;

	return EXIT_SUCCESS ;
}
