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

str \"({letter}|{number}|"_")+\"
identifier {letter}({letter}|{number}|"_")*
comment #.*\n
newline \n
whitespace [ \t]+

%%
"Reg" 			      { return T_REGDEF; }
"Code"			      { return T_CODE; }
"RegClass"	      { return T_REGCLDEF; }
"NBits"			      { return T_BITS; }
"Regs"			      { return T_REGS; }
"Instr"			      { return T_INSTRDEF; }
"Input"			      { return T_INPUT; }
"Output"		      { return T_OUTPUT; }
"Encoding"	      { return T_ENCODING; }
"Flags"           { return T_FLAGS; }
"Commutative"     { return T_COMMUTATIVE; }
"OverwriteInput"  { return T_OVERWRITEINPUT; }
"Match"           { return T_MATCHDEF; }
"IPattern"        { return T_IPATTERN; }
"OPattern"        { return T_OPATTERN; }
"RefNode"         { return T_REFNODE; }
"If"				      { return T_IF; }
"Then"			      { return T_THEN; }
"Else"			      { return T_ELSE; }

"Int"				      { return T_DTINT; }
"Bool"			      { return T_DTBOOL; }
"Bits"			      { return T_DTBSTR; }
"String"          { return T_DTSTR; }
"True"			      { return T_TRUE; }
"False"			      { return T_FALSE; }

{str}             { return T_STR; }
{bitstr}		      { return T_BSTR; }
{number}		      { return T_NUM; }
{identifier}	    { return T_ID; }
{comment} 	      { linenr++ ; }
{whitespace}	    { /* skip whitespace */ }
{newline}	        { linenr++ ;  }

"!="	{ return TNEQ; }
"=="	{ return T_EQUAL; }
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
