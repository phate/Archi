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

node* create_expression( nodetype ntype, node *e1, node *e2 )
{
	node *n = create_node( ntype, NULL, NULL, e1->linenr ) ;
	ARE_SIBLINGS( e1, e2 ) ;
	add_children( n, e1 ) ;

	return n ;	
}

%}

%token TREG TREG_CODE 
%token TREGCL TREGCL_BITS TREGCL_REGS
%token TINSTR TINSTR_INPUT TINSTR_OUTPUT TINSTR_IMM TINSTR_ENCODING
%token TIDENT TNUM TTRUE TFALSE TBITSTR TINT TBOOL TBITS TSECTSEP
%token TIF TTHEN TELSE TSHIFTL TSHIFTR TLTEQ TGTEQ TLAND TLOR TCONCAT TEQ TNEQ 

%%
ArchDesc			:	Sections												{ ast = create_node( ARCHDEF, NULL, NULL, linenr ) ;
																									add_children( ast, $1 ) ;}
							;
Sections			: RegSect TSECTSEP
								InstrSect TSECTSEP AuxSect			{ node *rs, *is, *au ;
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
RegSectDef		: TREGCL RegClDef									{ $$ = $2 ;}
							| TREG RegDef											{ $$ = $2 ;}
							;
RegDef				:	RegDefIdent ',' RegDef					{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| RegDefIdent											{ $$ = $1 ; }						
							;
RegDefIdent		: Id '{' RegBody '}'							{ $$ = create_node( REGDEF, strdup("Reg"),
																									create_regprop(), $1->linenr ) ;
																									ARE_SIBLINGS( $1, $3 ) ;
																									add_children( $$, $1 ) ;}
							;
RegBody				: TREG_CODE '=' TNUM							{ int32_t* i = malloc( sizeof(int32_t) ) ;
																									*i = strtol( yytext, 0, 10 ) ;
																									$$ = create_node( CODE, NULL, i, linenr ) ;}
							;
RegClDef			: RegClDefIdent ',' RegClDef			{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| RegClDefIdent										{ $$ = $1 ; }
							;
RegClDefIdent :	Id '{' RegClBody '}'						{ $$ = create_node( REGCLDEF, strdup("RegClass"),
																									create_regclprop(), $1->linenr ) ;
																									ARE_SIBLINGS( $1, $3 ) ;
																								  add_children( $$, $1 ) ;}
							;
RegClBody			: RegClBody ',' RegClProp					{ ARE_SIBLINGS($3, $1) ; $$ = $3 ; }
							| RegClProp												{ $$ = $1 ; }
							;
RegClProp			: TREGCL_BITS '=' TNUM						{ uint32_t* i = malloc( sizeof(uint32_t) ) ;
																								  *i = strtol( yytext, 0, 10 ) ;
																								  $$ = create_node( BITS, NULL, i, linenr ) ; }
							| TREGCL_REGS '=' '[' IdList ']'	{	int32_t *i = malloc(sizeof(int32_t)) ; *i = -1 ;
																									$$ = create_node( REGS, NULL, i, linenr ) ;
																									add_children( $$, $4 ) ;}
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
InstrSectDef	: TINSTR InstrDef										{ $$ = $2 ; }
							;
InstrDef			: InstrDefIdent ',' InstrDef				{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| InstrDefIdent											{ $$ = $1 ; }
							;
InstrDefIdent : Id '{' InstrBody '}'							{ $$ = create_node( INSTRDEF, strdup("Instruction"),
																										create_instrprop(), $1->linenr ) ;
																									  ARE_SIBLINGS( $1, $3 ) ;
																										add_children( $$, $1 ) ;}
							;
InstrBody			: InstrProp ',' InstrBody						{ ARE_SIBLINGS($1, $3) ; $$ = $1 ; }
							| InstrProp													{ $$ = $1 ; }
							;
InstrProp			: TINSTR_INPUT '=' '[' ETIdList ']'	{ $$ = create_node( INPUT, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							| TINSTR_OUTPUT '=' '[' ETIdList ']'{ $$ = create_node( OUTPUT, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							| TINSTR_IMM '=' '[' ETIdList ']'		{ $$ = create_node( IMMEDIATE, NULL, NULL, linenr ) ;
																										add_children( $$, $4 ) ;}
							|	TINSTR_ENCODING '=' Exp						{ $$ = create_node( ENCODING, NULL, NULL, linenr ) ;
																										add_children( $$, $3 ) ;}	
							;
ETIdList			: TIdList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
TIdList				: TId ',' TIdList										{ ARE_SIBLINGS($1, $3) ; $$ = $1 ; }
							| TId																{ $$ = $1 ; }
							;
TId						: TIDENT														{buffer[1] = strdup(yytext) ;}
								TIDENT														{$$ = create_node( TID, buffer[1], strdup(yytext), linenr ) ;}
							| TINT TIDENT												{$$ = create_node( TID, strdup("Int"), strdup(yytext), linenr );}
							| TBOOL TIDENT											{$$ = create_node( TID, strdup("Bool"), strdup(yytext), linenr );}
							| TBITS TIDENT											{$$ = create_node( TID, strdup("Bits"), strdup(yytext), linenr );}
							;
IdList				: Id ',' IdList											{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }
							| Id																{ $$ = $1 ; }
							;
Id						: TIDENT														{ $$ = create_node( ID, NULL, strdup(yytext), linenr ) ; }
							; 

AuxSect				: FctDef AuxSect										{ $$ = $1 ; if( $2 != NULL ){ ARE_SIBLINGS($$, $2) ;}}
							|																		{ $$ = 0 ; }
							;
FctDef				: TId Args '=' Exp									{ $$ = create_node( FCTDEF, NULL, create_fctprop(), $1->linenr ) ;
																										ARE_SIBLINGS( $1, $2 ) ; ARE_SIBLINGS( $2, $4 ) ;
																										add_children( $$, $1 ) ;}
							;
Args					: '(' ETIdList ')'									{ $$ = create_node( ARGS, NULL, NULL, linenr ) ;
                                                    add_children( $$, $2 ) ;}
							;
Exp						: TIF Exp1 TTHEN Exp TELSE Exp			{ $$ = create_node( IFTHENELSE, NULL, NULL, linenr ) ;
																										ARE_SIBLINGS( $2, $4 ) ; ARE_SIBLINGS( $4, $6 ) ;
                                                    add_children( $$, $2 ) ;}
							| Exp1															{ $$ = $1 ; } 
							;
Exp1					: Exp1 TLOR Exp2										{ $$ = create_expression( LOGICALOR, $1, $3 ) ; }
							| Exp2															{ $$ = $1 ; }
							;
Exp2					:	Exp2 TLAND Exp3										{ $$ = create_expression( LOGICALAND, $1, $3 ) ; }
							| Exp3															{ $$ = $1 ; }
							;
Exp3					: Exp3 TNEQ Exp4										{ $$ = create_expression( NOTEQUAL, $1, $3 ) ; }
							| Exp3 TEQ Exp4											{ $$ = create_expression( EQUAL, $1, $3 ) ; }
							| Exp4															{ $$ = $1 ; }
							;
Exp4					: Exp4 '<' Exp5											{ $$ = create_expression( LESSTHAN, $1, $3 ) ; }
							|	Exp4 '>' Exp5											{ $$ = create_expression( GREATERTHAN, $1, $3 ) ; }
							| Exp4 TLTEQ Exp5										{ $$ = create_expression( LESSTHANEQUAL, $1, $3 ) ; }
							| Exp4 TGTEQ Exp5										{ $$ = create_expression( GREATERTHANEQUAL, $1, $3 ) ; }
							| Exp5															{ $$ = $1 ; }
							;
Exp5					: Exp5 TSHIFTR Exp6									{ $$ = create_expression( SHIFTRIGHT, $1, $3 ) ; }
							| Exp5 TSHIFTL Exp6									{ $$ = create_expression( SHIFTLEFT, $1, $3 ) ; }
							| Exp6															{ $$ = $1 ; }
							;
Exp6					: Exp6 '+' Exp7											{ $$ = create_expression( PLUS, $1, $3 ) ; }
							| Exp6 '-' Exp7											{ $$ = create_expression( MINUS, $1, $3 ) ; }
							| Exp7															{ $$ = $1 ; }
							;
Exp7					: Exp7 '*' Exp8											{ $$ = create_expression( TIMES, $1, $3 ) ; }
							| Exp7 '/' Exp8											{ $$ = create_expression( DIVIDE, $1, $3 ) ; }
							| Exp7 '%' Exp8											{ $$ = create_expression( MOD, $1, $3 ) ; }
							| Exp8															{ $$ = $1 ; }
							;
Exp8					: Exp8 TCONCAT Exp9									{ $$ = create_expression( CONCATENATION, $1, $3 ) ; }
							| Exp9															{ $$ = $1 ; }
							;
Exp9					: Exp10 '[' Exp ':' Exp ']'					{ $$ = create_node( BITSLICE, NULL, NULL, linenr ) ;
																										ARE_SIBLINGS( $1, $3 ) ; ARE_SIBLINGS( $3, $5 ) ;
																										add_children( $$, $1 ) ;}
							| Exp10															{ $$ = $1 ; }
							;
Exp10					: Id '.' TREG_CODE									{ node *n = create_node( TID, "Int", "code", linenr ) ;
																										$$ = create_expression( PROPSELECTION, $1, n ) ; }
							| Exp11															{ $$ = $1 ; }
							;
Exp11					: Id '(' EExpList ')'								{ if( $3 == NULL ){
																											$$ = create_node( FCTCALL, NULL, NULL, $1->linenr ) ;
																											add_children( $$, $1 ) ;}
																										else $$ = create_expression( FCTCALL, $1, $3 ) ; }
							| Exp12															{ $$ = $1 ; }
							;
Exp12					: Id																{ $$ = $1 ; }
							| TNUM															{ $$ = create_node(NUMBER, strdup("Int"),strdup(yytext),linenr);}
							| TBITSTR														{ $$ = create_node( BITSTRING, strdup("Bits"),
                                                    strdup(yytext), linenr) ; }
							| TTRUE															{ $$ = create_node( BOOLEAN, strdup("Bool"),
                                                    strdup("true"), linenr ) ; }
							| TFALSE														{ $$ = create_node( BOOLEAN, strdup("Bool"),
                                                    strdup("false"), linenr ) ; }
							| '(' Exp ')'												{ $$ = $2 ; }
							;
EExpList			: ExpList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
ExpList				: Exp ',' ExpList										{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }								
							| Exp																{ $$ = $1 ; }
							;
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
