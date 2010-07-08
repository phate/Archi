%{
#include "ast/node.h"

#include <talloc.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define YYSTYPE archi_ast_node*

extern int yyerror(char*) ;
extern int yylex() ;
extern char* yytext ; 
extern unsigned int linenr ; 

static archi_ast_node* ast = NULL ;
static void* buffer[2] ;

static void archi_children_add( archi_ast_node *p, archi_ast_node *fc )
{
  archi_ast_node *s ;
  p->first_child = fc ;
  FOREACH_NEXT_SIBLING( fc, s ){
    s->parent = p ;
    if( s->next_sibling == NULL )
      p->last_child = s ;
  }

}

archi_ast_node* archi_expression_create( archi_ast_nodetype ntype, archi_ast_node *e1, archi_ast_node *e2 )
{
  archi_ast_node *n = archi_ast_node_create( ast, ntype, NULL, e1->linenr ) ;
	archi_ast_node_next_sibling_set( e1, e2 ) ;
	archi_children_add( n, e1 ) ;

	return n ;	
}

%}

%token T_REGDEF T_CODE 
%token T_REGCLDEF T_BITS T_REGS
%token T_INSTRDEF T_INPUT T_OUTPUT T_ENCODING
%token T_ID T_SEP T_NUM TTRUE TFALSE T_BSTR T_DTINT T_DTBOOL T_DTBSTR
%token TIF TTHEN TELSE TSHIFTL TSHIFTR TLTEQ TGTEQ TLAND TLOR T_DOT T_CONCAT TEQ TNEQ 

%%
ArchDesc			:	Sections												{ archi_ast_node_init( ast, NT_ARCHDEF, NULL, linenr ) ;
																									archi_children_add( ast, $1 ) ;}
							;
Sections			: RegSect T_SEP
								InstrSect /*TSECTSEP AuxSect*/		{ archi_ast_node *rs, *is, *au ;
                                                    rs = archi_ast_node_create( ast, NT_REGSECT, NULL, linenr ) ;
                                                    rs->attr.nt_regsect.nregcls = -1 ; 
                                                    is = archi_ast_node_create( ast, NT_INSTRSECT, NULL, linenr ) ;
																									//au = create_node( AUXSECT, NULL, NULL, linenr ) ;
																									archi_children_add( rs, $1 ) ;
																									archi_children_add( is, $3 ) ;
																									//add_children( au, $5 ) ;
																									archi_ast_node_next_sibling_set( rs, is ) ; 
																									//ARE_SIBLINGS( is, au ) ;
																									$$ = rs ;}
							;
RegSect				: RegSectDef ';' RegSect					{ archi_ast_node *s ;
																									FOREACH_NEXT_SIBLING( $1, s ){
																										if( s->next_sibling == NULL ){
																											archi_ast_node_next_sibling_set( s, $3 ) ;
																											break ;
																										}
																									}}
							| RegSectDef ';'									{ $$ = $1 ; }
							;
RegSectDef		: T_REGCLDEF RegClDef						  { $$ = $2 ;}
            	| T_REGDEF RegDef									{ $$ = $2 ;}
							;
RegDef				:	RegDefIdent ',' RegDef					{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
							| RegDefIdent											{ $$ = $1 ; }						
							;
RegDefIdent		: Id '{' RegBody '}'							{ $$ = archi_ast_node_create( ast, NT_REGDEF, "Reg", $1->linenr ) ;
                                                  archi_nt_regdef_attributes_init( &($$->attr.nt_regdef) ) ;
                                                  archi_ast_node_next_sibling_set( $1, $3 ) ;
                                                  archi_children_add( $$, $1 ) ; }
							;
RegBody				: T_CODE '=' T_NUM							  { $$ = archi_ast_node_create( ast, NT_CODE, NULL, linenr ) ;
                                                  $$->attr.nt_code.code = strtol( yytext, 0, 10 ) ; }        
							;
RegClDef			: RegClDefIdent ',' RegClDef			{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
							| RegClDefIdent										{ $$ = $1 ; }
							;
RegClDefIdent :	Id '{' RegClBody '}'						{ $$ = archi_ast_node_create( ast, NT_REGCLDEF, "RegClass", $1->linenr ) ;
                                                  archi_nt_regcldef_attributes_init( &($$->attr.nt_regcldef) ) ;
                                                  archi_ast_node_next_sibling_set( $1, $3 ) ;
                                                  archi_children_add( $$, $1 ) ; }                                                  
							;
RegClBody			: RegClBody ',' RegClProp					{ archi_ast_node_next_sibling_set($3, $1) ; $$ = $3 ; }
							| RegClProp												{ $$ = $1 ; }
							;
RegClProp			: T_BITS '=' T_NUM						    { $$ = archi_ast_node_create( ast, NT_BITS, NULL, linenr ) ;
                                                  $$->attr.nt_bits.bits = strtol( yytext, 0, 10 ) ; }
							| T_REGS '=' '[' IdList ']'	      {	$$ = archi_ast_node_create( ast, NT_REGS, NULL, linenr ) ;
                                                  $$->attr.nt_regs.nregs = -1 ;
                                                  archi_children_add( $$, $4 ) ; }  
							;
InstrSect			: InstrSectDef ';' InstrSect				{ archi_ast_node* s ;
																										FOREACH_NEXT_SIBLING( $1, s ){
																											if( s->next_sibling == NULL ){
																												archi_ast_node_next_sibling_set( s, $3 ) ;
																												break ;
																											}
																										}}
							| InstrSectDef ';'									{ $$ = $1 ; }
							;
InstrSectDef	: T_INSTRDEF InstrDef						    { $$ = $2 ; }
							;
InstrDef			: InstrDefIdent ',' InstrDef				{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
							| InstrDefIdent											{ $$ = $1 ; }
							;
InstrDefIdent : Id '{' InstrBody '}'							{ $$ = archi_ast_node_create( ast, NT_INSTRDEF, "Instruction", $1->linenr ) ;
																									  archi_nt_instrdef_attributes_init( &($$->attr.nt_instrdef) ) ;
                                                    archi_ast_node_next_sibling_set( $1, $3 ) ;
																										archi_children_add( $$, $1 ) ;}
							;
InstrBody			: InstrProp ',' InstrBody						{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
							| InstrProp													{ $$ = $1 ; }
							;
InstrProp			: T_INPUT '=' '[' ETIdList ']'	    { $$ = archi_ast_node_create( ast, NT_INPUT, NULL, linenr ) ;
                                                    archi_nt_input_attributes_init( &($$->attr.nt_input) ) ;
																										archi_children_add( $$, $4 ) ;}
							| T_OUTPUT '=' '[' ETIdList ']'     { $$ = archi_ast_node_create( ast, NT_OUTPUT, NULL, linenr ) ;
                                                    $$->attr.nt_output.nregs = -1 ;
																										archi_children_add( $$, $4 ) ;}
//							| TINSTR_IMM '=' '[' ETIdList ']'		{ $$ = create_node( IMMEDIATE, NULL, NULL, linenr ) ;
//																										add_children( $$, $4 ) ;}
							|	T_ENCODING '=' Exp8						    { $$ = archi_ast_node_create( ast, NT_ENCODING, NULL, linenr ) ;
																										archi_children_add( $$, $3 ) ;}	
							;
ETIdList			: TIdList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
TIdList				: TId ',' TIdList										{ archi_ast_node_next_sibling_set($1, $3) ; $$ = $1 ; }
							| TId																{ $$ = $1 ; }
							;
TId						: T_ID														  { buffer[1] = strdup(yytext) ; }
								T_ID														  { $$ = archi_ast_node_create( ast, NT_TID, buffer[1], linenr ) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
                                                    
							| T_DTINT T_ID										  { $$ = archi_ast_node_create( ast, NT_TID, "Int", linenr ) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
							| T_DTBOOL T_ID										  { $$ = archi_ast_node_create( ast, NT_TID, "Bool", linenr ) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
							| T_DTBSTR T_ID									    { $$ = archi_ast_node_create( ast, NT_TID, "Bits", linenr ) ;
							                                      $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
              ;
IdList				: Id ',' IdList											{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
							| Id																{ $$ = $1 ; }
							;
Id						: T_ID														  { $$ = archi_ast_node_create( ast, NT_ID, NULL, linenr ) ;
                                                    $$->attr.nt_id.id = talloc_strdup( $$, yytext ) ; }
							; 
/*
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
*/
Exp8					: Exp8 T_CONCAT Exp9								{ $$ = archi_expression_create( NT_CONCAT, $1, $3 ) ;
                                                    $$->attr.nt_concat.len = -1 ;}
							| Exp9															{ $$ = $1 ; }
							;
Exp9					: Exp10 '[' Exp8 ':' Exp8 ']'		    { $$ = archi_ast_node_create( ast, NT_BSLC, NULL, linenr ) ;
                                                    archi_nt_bslc_attributes_init( &($$->attr.nt_bslc) ) ;
                                                    archi_ast_node_next_sibling_set( $1, $3 ) ;
                                                    archi_ast_node_next_sibling_set( $3, $5 ) ;
																										archi_children_add( $$, $1 ) ;}
							| Exp10															{ $$ = $1 ; }
							;
Exp10					: Exp12 '.' Exp10									  { $$ = archi_expression_create( NT_DOT, $1, $3 ) ; }
							| Exp12															{ $$ = $1 ; }
							;
/*Exp11					: Id '(' EExpList ')'								{ if( $3 == NULL ){
																											$$ = create_node( FCTCALL, NULL, NULL, $1->linenr ) ;
																											add_children( $$, $1 ) ;}
																										else $$ = create_expression( FCTCALL, $1, $3 ) ; }
							| Exp12															{ $$ = $1 ; }
							;
*/
Exp12					: Id																{ $$ = $1 ; }
							| T_NUM															{ $$ = archi_ast_node_create( ast, NT_NUM, "Int", linenr ) ;
                                                    $$->attr.nt_num.num = strtol( yytext, 0, 10 ) ;}
					    | T_BSTR														{ $$ = archi_ast_node_create( ast, NT_BSTR, "Bits", linenr ) ;
                                                    archi_nt_bstr_attributes_init( &($$->attr.nt_bstr) ) ;
                                                    $$->attr.nt_bstr.bstr = talloc_strndup( $$, yytext+1, strlen(yytext)-2 ) ;
                                                    $$->attr.nt_bstr.len = strlen(yytext)-2 ;}
/*							| TTRUE															{ $$ = create_node( BOOLEAN, strdup("Bool"),
                                                    strdup("true"), linenr ) ; }
							| TFALSE														{ $$ = create_node( BOOLEAN, strdup("Bool"),
                                                    strdup("false"), linenr ) ; }
*/						| '(' Exp8 ')'										  { $$ = $2 ; }
							;
/*EExpList			: ExpList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
ExpList				: Exp ',' ExpList										{ ARE_SIBLINGS( $1, $3 ) ; $$ = $1 ; }								
							| Exp																{ $$ = $1 ; }
							;
*/
%%

archi_ast_node* archi_parse()
{
	ast = archi_ast_node_talloc( NULL ) ;

  yyparse() ;

	return ast ;
}

int yyerror( char* message )
{
	fprintf( stderr, "%d: %s -> %s\n", linenr, message, yytext ) ;
	TALLOC_FREE(ast) ;
  return 0 ;
}
