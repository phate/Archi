%{
#include "nodes.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define YYSTYPE node*

extern int yyerror(char*) ;
extern int yylex() ;
extern char* yytext ; 
extern unsigned int linenr ; 

static node* ast = NULL ;
static void* buffer[2] ;

void add_children( node *p, node *fc )
{
	if( p != NULL && fc != NULL ){ 
		node *n ;
		SET_FIRST_CHILD( p, fc ) ;
		FOREACH_SIBLING( fc, n ){ 
			n->parent = p ;
			if( n->next_sibling == NULL ) SET_LAST_CHILD( p, n ) ;
		}
	}
}

%}

%token REG REGCL REG_CODE REGCL_BITS REGCL_REGS INSTR INSTR_INPUT INSTR_OUTPUT INSTR_IMM
%token IDENT NUM TINT TBOOL TBITS SECTSEP

%%
ArchDesc			:	Sections												{ ast = create_node( ARCHDEF, NULL, NULL, linenr ) ;
																									add_children( ast, $1 ) ;}
							;
Sections			: RegSect SECTSEP
								InstrSect SECTSEP AuxSect				{ node *rs, *is, *au ;
																									rs = create_node( REGSECT, NULL, NULL, linenr ) ;
																									is = create_node( INSTRSECT, NULL, NULL, linenr ) ;
																									au = create_node( AUXSECT, NULL, NULL, linenr ) ;
																									add_children( rs, $1 ) ;
																									add_children( is, $3 ) ;
																									add_children( au, $5 ) ;
																									ARE_SIBLINGS( rs, is ) ;
																									ARE_SIBLINGS( is, au ) ;
																									$$ = rs ;}
							;
RegSect				: RegSectDef ';' RegSect					{ node* n ;
																									FOREACH_SIBLING( $1, n){
																										if( n->next_sibling == NULL ){
																											ARE_SIBLINGS( n, $3 ) ;
																											break ;
																										}
																									}}
							| RegSectDef ';'									{ $$ = $1 ;}
							;
RegSectDef		: REGCL RegClDef									{ $$ = $2 ;}
							| REG RegDef											{ $$ = $2 ;}
							;
RegDef				:	RegDefIdent ',' RegDef					{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| RegDefIdent											{ $$ = $1 ; }						
							;
RegDefIdent		: Id '{' RegBody '}'							{ $$ = create_node( REGDEF, strdup("Reg"),
																									create_regprop(), linenr ) ;
																									ARE_SIBLINGS( $1, $3 ) ;
																									add_children( $$, $1 ) ;}
							;
RegBody				: REG_CODE '=' NUM								{ int32_t* i = malloc( sizeof(int32_t) ) ;
																									*i = strtol( yytext, 0, 10 ) ;
																									$$ = create_node( CODE, NULL, i, linenr ) ;}
							;
RegClDef			: RegClDefIdent ',' RegClDef			{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| RegClDefIdent										{ $$ = $1 ; }
							;
RegClDefIdent :	Id '{' RegClBody '}'						{ $$ = create_node( REGCLDEF, strdup("RegClass"),
																									create_regclprop(), linenr ) ;
																									ARE_SIBLINGS( $1, $3 ) ;
																								  add_children( $$, $1 ) ;}
							;
RegClBody			: RegClBody ',' RegClProp					{ ARE_SIBLINGS($3, $1) ; $$ = $3 ; }
							| RegClProp												{ $$ = $1 ; }
							;
RegClProp			: REGCL_BITS '=' NUM							{ uint32_t* i = malloc( sizeof(uint32_t) ) ;
																								  *i = strtol( yytext, 0, 10 ) ;
																								  $$ = create_node( BITS, NULL, i, linenr ) ; }
							| REGCL_REGS '=' '[' RegList ']'	{ $$ = create_node( REGS, NULL, NULL, linenr ) ;
																									add_children( $$, $4 ) ;}
							;
RegList				: RegList ',' IDENT								{ $$ = create_node( ID, NULL, strdup(yytext), linenr ) ;
																									ARE_SIBLINGS( $$, $1 ) ;}
							| IDENT														{ $$ = create_node( ID, NULL, strdup(yytext), linenr ) ; }
							;

InstrSect			: InstrSectDef ';' InstrSect				{ node* n ;
																										FOREACH_SIBLING( $1, n ){
																											if( n->next_sibling == NULL ){
																												ARE_SIBLINGS( n, $3 ) ;
																												break ;
																											}
																										}}
							| InstrSectDef ';'									{ $$ = $1 ; }
							;
InstrSectDef	: INSTR InstrDef										{ $$ = $2 ; }
							;
InstrDef			: InstrDefIdent ',' InstrDef				{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| InstrDefIdent											{ $$ = $1 ; }
							;
InstrDefIdent : Id '{' InstrBody '}'							{ $$ = create_node( INSTRDEF, strdup("Instruction"),
																										create_instrprop(), linenr ) ;
																									  ARE_SIBLINGS( $1, $3 ) ;
																										add_children( $$, $1 ) ;}
							;
InstrBody			: InstrBody ',' InstrProp						{ ARE_SIBLINGS($3, $1) ; $$ = $3 ; }
							| InstrProp													{ $$ = $1 ; }
							;
InstrProp			: INSTR_INPUT '=' '[' ETIdList ']'	{ $$ = create_node( INPUT, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							| INSTR_OUTPUT '=' '[' ETIdList ']'	{ $$ = create_node( OUTPUT, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							| INSTR_IMM '=' '[' ETIdList ']'		{ $$ = create_node( IMMEDIATE, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							;
ETIdList			: TIdList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
TIdList				: TId ',' TIdList										{ ARE_SIBLINGS($1, $3) ; $$ = $1 ; }
							| TId																{ $$ = $1 ; }
							;
TId						: IDENT															{ buffer[1] = strdup(yytext) ;}
								IDENT															{ $$ = create_node( ID, buffer[1], strdup(yytext), linenr ) ;}
							| TINT IDENT												{ $$ = create_node( ID, strdup("Int"), strdup(yytext), linenr );}
							| TBOOL IDENT												{ $$ = create_node( ID, strdup("Bool"), strdup(yytext), linenr );}
							| TBITS IDENT												{	$$ = create_node( ID, strdup("Bits"), strdup(yytext), linenr );}
							;
Id						: IDENT															{ $$ = create_node( ID, NULL, strdup(yytext), linenr ) ; }
							; 

AuxSect				: FktDef AuxSect										{ $$ = $1 ; if( $2 != NULL ){ ARE_SIBLINGS($$, $2) ;}}
							|																		{ $$ = 0 ; }
							;
FktDef				: TId Args '=' Exp									{ $$ = create_node( FKTDEF, NULL, NULL, linenr ) ;
																										ARE_SIBLINGS( $1, $2 ) ; ARE_SIBLINGS( $2, $4 ) ;
																										add_children( $$, $1 ) ;}
							;
Args					: '(' ETIdList ')'									{ $$ = create_node( ARGS, NULL, NULL, linenr ) ; add_children( $$, $2 ) ;}
							;
/*
Exp						: IF Exp1 THEN Exp ELSE Exp					{}
							| Exp1															{} 
							;
Exp1					: Exp1 OR Exp2											{}
							| Exp2															{}
							;
Exp2					: 
*/
%%

node* parse()
{
	yyparse() ;

	return ast ;
}

int yyerror( char* message )
{
	fprintf( stderr, "%d: %s -> %s\n", linenr, message, yytext ) ;
	return 0 ;
}
