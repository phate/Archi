#include "parser.h"
#include "nodes.h"
#include "analyze.h"
#include "symtab.h"
#include "trim.h"
#include "ehandling.h"

#include <stdio.h>
#include <stdlib.h>

extern node* parse() ;

static void report_errors( node *n )
{
	uint32_t cnt = print_msgs( n ) ;
	if( cnt != 0 ) exit( EXIT_FAILURE ) ;
} 

int main()
{
	node* ast ;
	symtab stab = create_symtab() ;
	
	ast = parse() ;
	if( ast == NULL ) exit( EXIT_FAILURE ) ;
	
	trim_tree( ast ) ;

	fill_symtab( stab, ast ) ;
	report_errors( ast ) ;
	
	typecheck( stab, ast ) ;
	print_symtab( stab ) ;
	view_tree( ast ) ;
	report_errors( ast ) ;


	return 0 ;
}
