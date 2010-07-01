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
"Instr"			{ return T_INSTRDEF; }
"input"			{ return T_INPUT; }
"output"		{ return T_OUTPUT; }
"encoding"	{ return TINSTR_ENCODING; }
"if"				{ return TIF; }
"then"			{ return TTHEN; }
"else"			{ return TELSE; }

"Int"				{ return T_INT; }
"Bool"			{ return T_BOOL; }
"Bits"			{ return T_BSTR; }
"true"			{ return TTRUE; }
"false"			{ return TFALSE; }

{bitstr}		{ return T_BITSTR; }
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
