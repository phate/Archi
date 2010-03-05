%{
#include "parser.h"
#include <stdio.h>

unsigned int linenr = 1 ;
%}

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

"int"				{ return INT; }

{number}	{ return NUM; }
{identifier}	{ return IDENT; }
{comment} 	{ /* skip comments */ }
{whitespace}	{ /* skip whitespace */ }
{newline}	{ linenr++ ;  }

"="		{ return('='); }
","		{ return(','); }
"("		{ return('('); }
")"		{ return(')'); }
"{"		{ return('{'); }
"}"		{ return('}'); }
"["		{ return('['); }
"]"		{ return(']'); }
";"		{ return(';'); }
"%"		{ return('%'); }
"%%"	{ return SECTSEP; }
. 		{ printf("Error\n"); }
%%
/*
int main()
{
	yylex() ;
	return 0 ;
}
*/
