%{
#include <string>
#include "node.h"
#include "parser.hpp"

#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)
%}

%option noyywrap
%option yylineno

%%

[ \t\n]					        ;
";"                             return TOKEN(TSEMI);
"int"                           return TOKEN(TTYPE_INT);
"float"                         return TOKEN(TTYPE_FLOAT);
"return"				        return TOKEN(TRETURN);
"struct"                        return TOKEN(TSTRUCT);
"if"                            return TOKEN(TIF);
"else"                          return TOKEN(TELSE);
"while"                         return TOKEN(TWHILE);

[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TID;
[0-9]+\.[0-9]* 			    SAVE_TOKEN; return TFLOAT;
[0-9]+					        SAVE_TOKEN; return TINT;

"="						        return TOKEN(TASSIGN);
"=="				          	return TOKEN(TEQ);
"!="			          		return TOKEN(TNE);
"<"				          		return TOKEN(TLT);
"<="	          				return TOKEN(TLE);
">"				          		return TOKEN(TGT);
">="					        return TOKEN(TGE);

"!"                             return TOKEN(TNOT);
"&&"                            return TOKEN(TAND);
"||"                            return TOKEN(TOR);

"("	          					return TOKEN(TLP);
")"					          	return TOKEN(TRP);
"{"         				    return TOKEN(TLC);
"}"					          	return TOKEN(TRC);
"["                             return TOKEN(TLB);
"]"                             return TOKEN(TRB);
"."         					return TOKEN(TDOT);
","				          		return TOKEN(TCOMMA);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);

.                       printf("Unknown token!\n"); yyterminate();

%%
