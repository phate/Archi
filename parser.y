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
static archi_ast_node* tmp ;

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
%token T_INSTRDEF T_INPUT T_OUTPUT T_ENCODING T_FLAGS
%token T_MATCHDEF T_IPATTERN T_OPATTERN T_REFNODE
%token T_ANODEDEF T_MATCHES
%token T_COMMUTATIVE T_OVERWRITEINPUT
%token T_ID T_NUM T_TRUE T_FALSE T_BSTR T_STR
%token T_DTINT T_DTBOOL T_DTBSTR T_DTSTR
%token T_IF T_THEN T_ELSE T_SEP TSHIFTL TSHIFTR TLTEQ TGTEQ TLAND TLOR T_DOT T_CONCAT TNEQ 
%token T_EQUAL

%%
ArchDesc			:	Sections												{ archi_ast_node_init( ast, NT_ARCHDEF, NULL, linenr ) ;
																									archi_nt_archdef_attributes_init( &(ast->attr.nt_archdef) ) ;
                                                  archi_children_add( ast, $1 ) ;}
							;
Sections			: RegSect T_SEP
								InstrSect T_SEP MatchSect		    { archi_ast_node *rs, *is, *au, *ps ;
                                                  rs = archi_ast_node_create( ast, NT_REGSECT, NULL, linenr ) ;
                                                  rs->attr.nt_regsect.nregcls = -1 ; 
                                                  is = archi_ast_node_create( ast, NT_INSTRSECT, NULL, linenr ) ;
																								  ps = archi_ast_node_create( ast, NT_PATTERNSECT, NULL, linenr ) ;
                                                  archi_nt_patternsect_attributes_init( &(ps->attr.nt_patternsect) ) ;
                                                    //au = create_node( AUXSECT, NULL, NULL, linenr ) ;
																									archi_children_add( rs, $1 ) ;
																									archi_children_add( is, $3 ) ;
                                                  archi_children_add( ps, $5 ) ;
																									//add_children( au, $5 ) ;
																									archi_ast_node_next_sibling_set( rs, is ) ;
                                                  archi_ast_node_next_sibling_set( is, ps ) ; 
																									//ARE_SIBLINGS( is, au ) ;
																									$$ = rs ;}
							;
MatchSect     : MatchSectDef ';' MatchSect      { archi_ast_node *s ;
                                                  FOREACH_NEXT_SIBLING( $1, s){
                                                    if( s->next_sibling == NULL ){
                                                      archi_ast_node_next_sibling_set( s, $3 ) ;
                                                      break ;
                                                    }
                                                  }}
              | MatchSectDef ';'                { $$ = $1 ; }
              ;
MatchSectDef  : T_MATCHDEF MatchDef             { $$ = $2 ; }
              | T_ANODEDEF ANodeDef              { $$ = $2 ; }
              ;
ANodeDef      : ANodeDefIdent ',' ANodeDef      { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | ANodeDefIdent                   { $$ = $1 ; }
              ;
ANodeDefIdent : Id '{' ANodeBody '}'            { $$ = archi_ast_node_create( ast, NT_ANODEDEF, "ANode", $1->linenr) ;
                                                  archi_nt_anodedef_attributes_init( &($$->attr.nt_anodedef) ) ;
                                                  archi_ast_node_next_sibling_set( $1, $3 ) ;
                                                  archi_children_add( $$, $1 ) ; }
              ;
ANodeBody     : ANodeProp ',' ANodeBody         { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | ANodeProp                       { $$ = $1 ; }
              ;
ANodeProp     : T_MATCHES '=' '[' IdList ']'    { $$ = archi_ast_node_create( ast, NT_MATCHES, NULL, linenr ) ;
                                                  archi_children_add( $$, $4 ) ; }
              ;
MatchDef      : MatchDefIdent ',' MatchDef      { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | MatchDefIdent                   { $$ = $1 ; }
              ;
MatchDefIdent : Id '{' MatchBody '}'            { $$ = archi_ast_node_create( ast, NT_MATCHDEF, "Match", $1->linenr ) ;
                                                  archi_nt_matchdef_attributes_init( &($$->attr.nt_matchdef) ) ;
                                                  archi_ast_node_next_sibling_set( $1, $3 ) ;
                                                  archi_children_add( $$, $1 ) ; }
              ;
MatchBody     : MatchProp ',' MatchBody         { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | MatchProp                       { $$ = $1 ; }
              ;
MatchProp     : T_INPUT '=' '[' ETIdList ']'    { $$ = archi_ast_node_create( ast, NT_INPUT, NULL, linenr ) ;
                                                  $$->attr.nt_input.nchildren = -1 ;
                                                  archi_children_add( $$, $4 ) ; }
              | T_OUTPUT '=' '[' ETIdList ']'   { $$ = archi_ast_node_create( ast, NT_OUTPUT, NULL, linenr ) ;
                                                  $$->attr.nt_output.nchildren = -1 ;
                                                  archi_children_add( $$, $4 ) ; }
              | T_IPATTERN '=' '[' NodeList ']' { $$ = archi_ast_node_create( ast, NT_IPATTERN, NULL, linenr ) ;
                                                  archi_children_add( $$, $4 ) ; }
              | T_OPATTERN '=' '[' NodeList ']' { $$ = archi_ast_node_create( ast, NT_OPATTERN, NULL, linenr ) ;
                                                  archi_children_add( $$, $4 ) ; }
              | T_REFNODE '=' Id                { $$ = archi_ast_node_create( ast, NT_REFNODE, NULL, $3->linenr ) ;
                                                  archi_nt_refnode_attributes_init( &($$->attr.nt_refnode) ) ;
                                                  archi_children_add( $$, $3) ; }
              ;
NodeList      : NodeDef ',' NodeList            { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | NodeDef                         { $$ = $1 ; }
              ;
NodeDef       : IdList '=' Id '[' T_ID          { tmp = archi_ast_node_create( ast, NT_NODEDEF, yytext, linenr ) ;
                                                  archi_nt_nodedef_attributes_init( &(tmp->attr.nt_nodedef) ) ; }
                ']' '(' EExpList ')'            { archi_ast_node *ip = archi_ast_node_create( ast, NT_INPUT, NULL, 0 ) ;
                                                  ip->attr.nt_input.nchildren = -1 ;
                                                  archi_ast_node *op = archi_ast_node_create( ast, NT_OUTPUT, NULL, 0 ) ;
                                                  op->attr.nt_output.nchildren = -1 ;
                                                  archi_children_add( ip, $9 ) ; archi_children_add( op, $1 ) ;
                                                  archi_ast_node_next_sibling_set( $3, ip ) ;
                                                  archi_ast_node_next_sibling_set( ip, op ) ;
                                                  archi_children_add( tmp, $3 ) ; $$ = tmp ; }
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
                                                    $$->attr.nt_input.nchildren = -1 ;
																										archi_children_add( $$, $4 ) ;}
							| T_OUTPUT '=' '[' ETIdList ']'     { $$ = archi_ast_node_create( ast, NT_OUTPUT, NULL, linenr ) ;
                                                    $$->attr.nt_output.nchildren = -1 ;
																										archi_children_add( $$, $4 ) ;}
//							| TINSTR_IMM '=' '[' ETIdList ']'		{ $$ = create_node( IMMEDIATE, NULL, NULL, linenr ) ;
//																										add_children( $$, $4 ) ;}
							|	T_ENCODING '=' Exp						    { $$ = archi_ast_node_create( ast, NT_ENCODING, NULL, linenr ) ;
																									  $$->attr.nt_encoding.nifthenelse = -1 ;	
                                                    archi_children_add( $$, $3 ) ; }
              | T_FLAGS '=' '[' FlagList ']'      { $$ = archi_ast_node_create( ast, NT_FLAGS, NULL, linenr ) ;
                                                    $$->attr.nt_flags.flags = 0 ;
                                                    archi_children_add( $$, $4 ) ; }
							;
FlagList      : Flag ',' FlagList                 { archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }
              | Flag                              { $$ = $1 ; }
              ;
Flag          : T_OVERWRITEINPUT                  { $$ = archi_ast_node_create( ast, NT_OVERWRITEINPUTFLAG, NULL, linenr ) ; }
              | T_COMMUTATIVE                     { $$ = archi_ast_node_create( ast, NT_COMMUTATIVEFLAG, NULL, linenr ) ; }
              ;
ETIdList			: TIdList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
TIdList				: TId ',' TIdList										{ archi_ast_node_next_sibling_set($1, $3) ; $$ = $1 ; }
							| TId																{ $$ = $1 ; }
							;
TId           : T_ID                              { tmp = archi_ast_node_create( ast, NT_TID, yytext, linenr ) ; }
                T_ID                              { tmp->attr.nt_tid.id = talloc_strdup( tmp, yytext ) ; $$ = tmp ; }
							| T_DTINT T_ID										  { $$ = archi_ast_node_create( ast, NT_TID, "Int", linenr ) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
							| T_DTBOOL T_ID										  { $$ = archi_ast_node_create( ast, NT_TID, "Bool", linenr ) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
							| T_DTBSTR T_ID									    { $$ = archi_ast_node_create( ast, NT_TID, "Bits", linenr ) ;
							                                      $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
              | T_DTSTR T_ID                      { $$ = archi_ast_node_create( ast, NT_TID, "String", linenr) ;
                                                    $$->attr.nt_tid.id = talloc_strdup( $$, yytext ) ; }
              ;
//EIdList       : IdList                            { $$ = $1 ; }
//              |                                   { $$ = NULL ; }
//              ;
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
*/
Exp					  : T_IF Exp3 T_THEN Exp T_ELSE Exp   { $$ = archi_ast_node_create( ast, NT_IFTHENELSE, NULL, linenr ) ;
                                                    archi_nt_ifthenelse_attributes_init( &($$->attr.nt_ifthenelse) ) ;
																										archi_ast_node_next_sibling_set( $2, $4 ) ;
                                                    archi_ast_node_next_sibling_set( $4, $6 ) ;
                                                    archi_children_add( $$, $2 ) ;}
							| Exp3															{ $$ = $1 ; } 
							;
/*
Exp1					: Exp1 TLOR Exp2										{ $$ = create_expression( LOGICALOR, $1, $3 ) ; }
							| Exp2															{ $$ = $1 ; }
							;
Exp2					:	Exp2 TLAND Exp3										{ $$ = create_expression( LOGICALAND, $1, $3 ) ; }
							| Exp3															{ $$ = $1 ; }
							;
*/
//Exp3					: Exp3 TNEQ Exp4										{ $$ = create_expression( NOTEQUAL, $1, $3 ) ; }
Exp3				  : Exp3 T_EQUAL Exp8							    { $$ = archi_expression_create( NT_EQUAL, $1, $3 ) ; }
							| Exp8															{ $$ = $1 ; }
							;
/*
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
Exp8				  : Exp8 T_CONCAT Exp9							  { $$ = archi_expression_create( NT_CONCAT, $1, $3 ) ;
                                                    $$->attr.nt_concat.len = -1 ;}
							| Exp9															{ $$ = $1 ; }
							;
Exp9					: Exp10 '[' Exp9 ':' Exp9 ']'		    { $$ = archi_ast_node_create( ast, NT_BSLC, NULL, linenr ) ;
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
              | T_STR                             { $$ = archi_ast_node_create( ast, NT_STR, "String", linenr ) ;
                                                    archi_nt_str_attributes_init( &($$->attr.nt_str) ) ;
                                                    if( strlen(yytext) > 2 ){
                                                      $$->attr.nt_str.str = talloc_strndup( $$, yytext+1, strlen(yytext)-2 ) ;
                                                      $$->attr.nt_str.length = strlen(yytext)-2 ;
                                                    }}
							| T_TRUE													  { $$ = archi_ast_node_create( ast, NT_TRUE, "Bool", linenr ) ; }
							| T_FALSE														{ $$ = archi_ast_node_create( ast, NT_FALSE, "Bool", linenr ) ; }
						  | '(' Exp ')'										    { $$ = $2 ; }
              ;
EExpList			: ExpList														{ $$ = $1 ; }
							|																		{ $$ = NULL ; }
							;
ExpList				: Exp ',' ExpList										{ archi_ast_node_next_sibling_set( $1, $3 ) ; $$ = $1 ; }								
							| Exp																{ $$ = $1 ; }
							;

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
