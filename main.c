#include "parser.h"
#include "nodes.h"
#include "analyze.h"
#include "symtab.h"
#include "trim.h"
#include "ehandling.h"

#include <stdio.h>
#include <stdlib.h>

extern node* parse() ;
extern FILE *yyin ;
FILE *ad = NULL ;
FILE *hf = NULL ;
FILE *sf = NULL ;

static void cleanup()
{
	//TODO: more todo
	fclose( ad ) ;
	fclose( hf ) ;
	fclose( sf ) ;
}

static FILE* open_file( const char *filename, const char *mode )
{
	FILE *f = fopen( filename, mode ) ;
	if( f == NULL ){
		fprintf( stderr, "Cannot open file: %s", filename ) ;
		cleanup() ;
		exit( EXIT_FAILURE ) ; 
	}

	return f ;
}

static void process_input( int argc, char* argv[] )
{
	if( argc != 4 ){
		fprintf( stderr, "Invalid number of arguments:\n" ) ;
		fprintf( stderr, "%s <arch description> <output header file> <output source file>\n", argv[0] ) ;
		exit( EXIT_FAILURE ) ;
	}

	ad = open_file( argv[1], "r" ) ;
	hf = open_file( argv[2], "w" ) ;
	sf = open_file( argv[3], "w" ) ;	
} 

static void report_errors( node *n )
{
	uint32_t cnt = print_msgs( n ) ;
	if( cnt != 0 ){
		cleanup() ;
	 	exit( EXIT_FAILURE ) ;
	}
}

int main( int argc, char* argv[] )
{
	process_input( argc, argv ) ;
	yyin = ad ;

	node* ast ;
	symtab stab = create_symtab() ;
	
	ast = parse() ;
	if( ast == NULL ){
		cleanup() ;
		exit( EXIT_FAILURE ) ;
	}
	view_tree(ast) ;
	
	trim_tree( ast ) ;
	fill_symtab( stab, ast ) ;
	report_errors( ast ) ;
	
	print_symtab( stab ) ;
	view_tree( ast ) ;
	typecheck( stab, ast ) ;
	report_errors( ast ) ;

	cleanup() ;
	return 0 ;
}
