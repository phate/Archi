%{
#include "parser.h"
#include <stdio.h>

unsigned int linenr = 1 ;

extern int yyerror( char* message ) ;
%}

bitstr \'(0|1)*\'
digit [0-9]+
number {digit}+
letter [a-zA-Z]

identifier {letter}({letter}|{number}|"_")*
comment #.*\n
newline \n
whitespace [ \t]+

%%
"Reg" 			{ return T_REGDEF; }
"Code"			{ return T_CODE; }
"RegClass"	{ return T_REGCLDEF; }
"NBits"			{ return T_BITS; }
"Regs"			{ return T_REGS; }
"Instr"			{ return T_INSTRDEF; }
"Input"			{ return T_INPUT; }
"Output"		{ return T_OUTPUT; }
"Encoding"	{ return T_ENCODING; }
"Match"     { return T_MATCHDEF; }
"IPattern"  { return T_IPATTERN; }
"OPattern"  { return T_OPATTERN; }
"if"				{ return TIF; }
"then"			{ return TTHEN; }
"else"			{ return TELSE; }

"Int"				{ return T_DTINT; }
"Bool"			{ return T_DTBOOL; }
"Bits"			{ return T_DTBSTR; }
"true"			{ return TTRUE; }
"false"			{ return TFALSE; }

{bitstr}		{ return T_BSTR; }
{number}		{ return T_NUM; }
{identifier}	{ return T_ID; }
{comment} 	{ linenr++ ; }
{whitespace}	{ /* skip whitespace */ }
{newline}	{ linenr++ ;  }

"!="	{ return TNEQ; }
"=="	{ return TEQ; }
"++"	{ return T_CONCAT; }
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
