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
"Reg" 			{ return REG; }
"RegClass"	{ return REGCL; }
"code"			{ return REG_CODE; }
"bits"			{ return REGCL_BITS; }
"regs"			{ return REGCL_REGS; }
"Instr"			{ return INSTR; }
"input"			{ return INSTR_INPUT; }
"output"		{ return INSTR_OUTPUT; }
"immediate"	{ return INSTR_IMM; }
"encoding"	{ return INSTR_ENCODING; }
"if"				{ return IF; }
"then"			{ return THEN; }
"else"			{ return ELSE; }

"Int"				{ return TINT; }
"Bool"			{ return TBOOL; }
"Bits"			{ return TBITS; }
"true"			{ return TRUE; }
"false"			{ return FALSE; }

{bitstr}		{ return BITSTR; }
{number}		{ return NUM; }
{identifier}	{ return IDENT; }
{comment} 	{ linenr++ ; }
{whitespace}	{ /* skip whitespace */ }
{newline}	{ linenr++ ;  }

"!="	{ return NEQ; }
"=="	{ return EQ; }
"++"	{ return CONCAT; }
"||"	{ return LOR; }
"&&"	{ return LAND; }
">>"	{ return SHIFTR; }
"<<"	{ return SHIFTL; }
">="	{ return GTEQ; }
"<="	{ return LTEQ; }

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
"%%"	{ return SECTSEP; }
. 		{ yyerror("syntax error") ; exit( EXIT_FAILURE ) ; }
%%
