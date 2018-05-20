%{
    #include "node.h"
    #include <cstdio>
    #include <cstdlib>
    NBlock* programBlock; /* the top level root node of our final AST */

    extern int yylex();
    extern int yylineno;
    void yyerror(const char *s) { std::printf("Error type B at Line %d: \'%s\'\n", yylineno, s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
    NStmt* Stmt;
    NStructMem *StructMem;
    NExp* Exp;
    NSpecifier* Specifier;
    NVarDec* VarDec;
    NFuncDec* FuncDec;
    NStructSpecifier* StructSpecifier;
    NIdentifier* Identifier;
    NDef* Def;
    NParamDec* ParamDec;
    NCompSt* Compst;
    NExtDef* ExtDef;
    NDec* Dec;
    NStmtList* StmtList;
    NExpList* ExpList;
    NExtDefList* ExtDefList;
    NDefList* DefList;
    NDecList* DecList;
    NExtDecList* ExtDecList;
    NVarList* VarList;
    std::string *string;
    NBlock* Block;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TID TINT TFLOAT
%token <token> TEQ TNE TLT TLE TGT TGE TASSIGN
%token <token> TAND TOR TNOT
%token <token> TLP TRP TLB TRB TLC TRC TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TRETURN TIF TELSE TWHILE
%token <token> TTYPE_INT TTYPE_FLOAT TSTRUCT TSEMI

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */

%type <Dec> Dec
%type <Stmt> Stmt 
%type <Exp> Exp 
%type <ExtDef> ExtDef
%type <Specifier> Specifier
%type <VarDec> VarDec
%type <FuncDec> FuncDec
%type <StructSpecifier> StructSpecifier
%type <Identifier> OptTag
%type <Def> Def
%type <ParamDec> ParamDec
%type <Compst> CompSt
%type <StmtList> StmtList
%type <ExtDefList> ExtDefList
%type <Block> Program
%type <DefList> DefList
%type <DecList> DecList
%type <VarList> VarList
%type <ExtDecList> ExtDecList
%type <ExpList> Args

/* Operator precedence for mathematical operators */

%right TCOMMA
%right TASSIGN
%left TAND TOR
%left TEQ TNE TLT TLE TGT TGE
%left TPLUS TMINUS
%left TMUL TDIV
%right TNOT
%left TLP TRP TLB TRB TDOT
%start Program

%nonassoc TELSE

%%

Program : ExtDefList {$$=new NBlock($1->lineno,*$1);programBlock=$$;}
        ;

ExtDefList:/* */{$$=new NExtDefList(yylineno);}
          | ExtDef ExtDefList {$$=$2;$$->lineno=$1->lineno;$2->push_front($1);}
          ;

ExtDef    : Specifier ExtDecList TSEMI {$$=new NExtDefNormal($1->lineno,*$1,*$2);}
          | Specifier TSEMI {$$=new NExtDefNormal($1->lineno,*$1);}
          | Specifier FuncDec CompSt {$$=new NExtDefFunc($1->lineno,*$1,*$2,*$3);}
          ;

ExtDecList: VarDec {$$=new NExtDecList($1->lineno);$$->push_front($1);}
          | VarDec TCOMMA ExtDecList {$$=$3;$$->lineno=$1->lineno;$3->push_front($1);}
          ;

Specifier : TTYPE_INT {$$=new NSpecifier(yylineno,$<token>1);}
          | TTYPE_FLOAT {$$=new NSpecifier(yylineno,$<token>1);}
          | StructSpecifier {$$=new NSpecifier($1->lineno,*$1);}
          ;

StructSpecifier:TSTRUCT OptTag TLC DefList TRC { $$=new NStructSpecifier($2->lineno,*$2,*$4);}
               | TSTRUCT TID {$$=new NStructSpecifier(yylineno,*$2);}
          ;

OptTag : /* blank */ {}
       | TID { $$ = new NIdentifier(yylineno,*$1);}
          ;

VarDec : TID {$$=new NVarDec(yylineno,*$1);}
       | VarDec TLB TINT TRB {$$=new NVarDec($1->lineno,*$1,atol($3->c_str()));}
       ;

FuncDec : TID TLP VarList TRP {$$=new NFuncDec($3->lineno,*(new NIdentifier(yylineno,*$1)),*$3);}
        | TID TLP TRP {$$=new NFuncDec(yylineno,*(new NIdentifier(yylineno,*$1)));}
       ;

VarList :ParamDec TCOMMA VarList {$$=$3;$$->lineno=$1->lineno;$$->push_front($1);}
        |ParamDec {$$=new NVarList();$$->lineno=$1->lineno;$$->push_front($1);}
       ;

ParamDec: Specifier VarDec {$$=new NParamDec($1->lineno,*$1,*$2);}
        ;

CompSt : TLC DefList StmtList TRC {$$=new NCompSt(yylineno);$$->lineno=$2->lineno;$$->add(*$2,*$3);}
       ;

StmtList :/* blank */ {$$=new NStmtList(yylineno);}
         | Stmt StmtList {$$=$2;$$->lineno=$1->lineno;$2->push_front($1);}
         ;
/*$$=new NExp($1->lineno,*$1);*/
Stmt  : Exp TSEMI {$$=new NStmt($1->lineno,*$$,1);}
      | CompSt {$$=new NCompSt($1->lineno,*$1);$$=new NStmt($1->lineno,*$$,0);}
      | TRETURN Exp TSEMI {$$=new NReturnStmt($2->lineno,*$2);$$=new NStmt($2->lineno,*$$,2);}
      | TIF TLP Exp TRP Stmt {$$=new NIfStmt($3->lineno,*$3,*$5);$$=new NStmt($3->lineno,*$$,0);}
      | TIF TLP Exp TRP Stmt TELSE Stmt {$$=new NIfStmt($3->lineno,*$3,*$5,*$7);$$=new NStmt($3->lineno,*$$,0);}
      | TWHILE TLP Exp TRP Stmt {$$=new NWhileStmt($3->lineno,*$3,*$5);$$=new NStmt($3->lineno,*$$,0);}
      ;

DefList:/*blank */ {$$=new NDefList(yylineno);} 
       | Def DefList {$$=$2;$$->lineno=$1->lineno;$$->push_front($1);}
       ;

Def  :Specifier DecList TSEMI {$$=new NDef($1->lineno,*$1,*$2);}
     ;

DecList : Dec {$$=new NDecList($1->lineno);$$->push_front($1);}
        | Dec TCOMMA DecList {$$=$3;$$->lineno=$1->lineno;$3->push_front($1);}
        ;

Dec    : VarDec {$$=new NDec($1->lineno,*$1);}
       | VarDec TASSIGN Exp {$$=new NDec($1->lineno,*$1,*$3);}
       ;

Exp  : Exp TASSIGN Exp { $$ = new NAssignment($1->lineno,*$1, *$3); $$=new NExp($1->lineno,*$$,0);}
     | Exp TAND Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TOR Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TMUL Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TDIV Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TMINUS Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TPLUS Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TEQ Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TNE Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TLT Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TLE Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TGE Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | Exp TGT Exp {$$ = new NBinaryOperator($1->lineno,*$1, $2, *$3);$$=new NExp($1->lineno,*$$,0);}
     | TLP Exp TRP {$$=new NExp($2->lineno,*$2,EUSELESS|$2->type);}
     | TMINUS Exp {$$=new NUnaryOperator($2->lineno,*$2,$1);$$=new NExp($2->lineno,*$$,0);}
     | TNOT Exp {$$=new NUnaryOperator($2->lineno,*$2,$1);$$=new NExp($2->lineno,*$$,0);}
     | TID TLP Args TRP {$$ = new NMethodCall($3->lineno,*(new NIdentifier(yylineno,*$1)), *$3);$$=new NExp($3->lineno,*$$,0);}
     | TID TLP TRP {$$ = new NMethodCall(yylineno,*(new NIdentifier(yylineno,*$1)));$$=new NExp(yylineno,*$$,0);}
     | Exp TLB Exp TRB {$$=new NArrayIndex($1->lineno,*$1,*$3);$$=new NExp($1->lineno,*$$,ERVAL|($1->type&(EFLOAT|EINT))) ;}
     | Exp TDOT TID {$$=new NStructMem($1->lineno,*$1,*$3);$$=new NExp($1->lineno,*$$,ERVAL);}
     | TID {$$=new NIdentifier(yylineno,*$1);$$=new NExp(yylineno,*$$,ERVAL|EID);}
     | TINT {$$ = new NInteger(yylineno,*$1);$$=new NExp(yylineno,*$$,EINT);}
     | TFLOAT {$$ = new NDouble(yylineno,atof($1->c_str()));$$=new NExp(yylineno,*$$,EFLOAT);}
     ;

Args : Exp {$$=new NExpList($1->lineno);$$->push_front($1);}
     | Exp TCOMMA Args {$$=$3;$$->lineno=$1->lineno;$$->push_front($1);}
     ;

%%
