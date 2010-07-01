%{
#include "parser.h"
#include <stdio.h>

unsigned int linenr = 1 ;

extern int yyerror( char* message ) ;
%}

bitstr \"(0|1)*\"
digit [0-9]+
number {digit}+
letter [a-zA-Z]

identifier {letter}({letter}|{number}|"_")*
comment #.*\n
newline \n
whitespace [ \t]+

%%
"Reg" 			{ return T_REGDEF; }
"code"			{ return T_CODE; }
"RegClass"	{ return T_REGCLDEF; }
"bits"			{ return T_BITS; }
"regs"			{ return T_REGS; }
"Instr"			{ return TINSTR; }
"input"			{ return TINSTR_INPUT; }
"output"		{ return TINSTR_OUTPUT; }
"immediate"	{ return TINSTR_IMM; }
"encoding"	{ return TINSTR_ENCODING; }
"if"				{ return TIF; }
"then"			{ return TTHEN; }
"else"			{ return TELSE; }

"Int"				{ return TINT; }
"Bool"			{ return TBOOL; }
"Bits"			{ return TBITS; }
"true"			{ return TTRUE; }
"false"			{ return TFALSE; }

{bitstr}		{ return TBITSTR; }
{number}		{ return T_NUM; }
{identifier}	{ return T_ID; }
{comment} 	{ linenr++ ; }
{whitespace}	{ /* skip whitespace */ }
{newline}	{ linenr++ ;  }

"!="	{ return TNEQ; }
"=="	{ return TEQ; }
"++"	{ return TCONCAT; }
"||"	{ return TLOR; }
"&&"	{ return TLAND; }
">>"	{ return TSHIFTR; }
"<<"	{ return TSHIFTL; }
">="	{ return TGTEQ; }
"<="	{ return TLTEQ; }

":"		{ return(':'); }
"."		{ return('.'); }
"+"		{ return('+'); }
"-"		{ return('-'); }
"/"		{ return('/'); }
"*"		{ return('*'); }
"%"		{ return('%'); }
">"		{ return('>'); }
"<"		{ return('<'); }
"="		{ return('='); }
","		{ return(','); }
"("		{ return('('); }
")"		{ return(')'); }
"{"		{ return('{'); }
"}"		{ return('}'); }
"["		{ return('['); }
"]"		{ return(']'); }
";"		{ return(';'); }
"%%"	{ return T_SEP; }
. 		{ yyerror("syntax error") ; exit( EXIT_FAILURE ) ; }
%%
