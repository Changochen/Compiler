%{
    #include "node.h"
    #include <cstdio>
    #include <cstdlib>
    std::vector<NExtDef*>* programBlock; /* the top level root node of our final AST */

extern int yylex();
    void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
    NStmt* Stmt;
    NExp* Exp;
    NSpecifier* Specifier;
    NVarDec* VarDec;
    NFuncDec* FuncDec;
    NStructSpecifier* StructSpecifier;
    NIdentifier* Identifier;
    NDef* Def;
    NParamDec* ParamDec;
    NCompst* Compst;
    NExtDef* ExtDef;
    NDec* Dec;
    std::vector<NStmt*>* StmtList;
    std::vector<NExp*>* ExpList;
    std::vector<NExtDef*>* ExtDefList;
    std::vector<NDef*>* DefList;
    std::vector<NDec*>* DecList;
    std::vector<NVarDec*>* VarDeclist
    std::vector<NParamDec*>* VarList;
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
%type <Exp>  Exp 
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
%type <ExtDefList> ExtDefList Program
%type <DefList> DefList
%type <DecList> DecList
%type <VarList> VarList
%type <VarDecList> ExtDecList
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

Program : ExtDefList { programBlock=*$1;}
        ;

ExtDefList:/* */{$$=new ExtDefList();}
          | ExtDef ExtDefList {$2->push_back(*$1);}
          ;
/*
          | ExtDef {$$=new ExtDefList();$$->push_back(*$1);}
          | ExtDefList ExtDef {$1->push_back(*$2);}
          ;
*/
ExtDef    : Specifier ExtDecList TSEMI {$$=new NExtDefNormal(*$1,*$2);}
          | Specifier TSEMI {$$=new NExtDefNormal(*$1);}
          | Specifier FuncDec CompSt {$$=new NExtDefFunc(*$1,*$2,*$3)}
          ;

ExtDecList: VarDec {$$=new VarDeclist();$$->push_back($1);}
          | VarDec TCOMMA ExtDecList {$3->push_back($1);}
          ;

Specifier : TTYPE_INT {$$=new NSepcifier($1);}
          | TTYPE_FLOAT {$$=new NSepcifier($1);}
          | StructSpecifier {$$=new NSepcifier(*$1);}
          ;

StructSpecifier:TSTRUCT OptTag TLC DefList TRC { $$=new NStructSpecifier(*$2,*$4);}
               | TSTRUCT TID {$$=new NStructSpecifier(*$2);}
          ;

OptTag : /* blank */ {}
       | TID { $$ = new NIdentifier(*$1);}
          ;

VarDec : TID {$$=new NVarDec(*$1);}
       | VarDec TLB TINT TRB {$$=new NVarDec(*$1,$3);}
       ;

FuncDec : TID TLP VarList TRP {$$=new NFuncDec(*$1,*$3);}
        | TID TLP TRP {$$=new NFuncDec(*$1);}
       ;

VarList :ParamDec TCOMMA VarList {$3->push_back(*$1);}
        |ParamDec {$$=new Varlist();$$->push_back(*$1);}
       ;

ParamDec: Specifier VarDec {$$=new ParamDec(*$1,*$2);}
        ;

CompSt : TLC DefList StmtList TRC {$$=new NCompSt();$$->add(*$1,*$2);}
       ;

StmtList :/* blank */ {$$=new StmtList();}
         | Stmt StmtList {$2->push_back(*$1);}
         ;

Stmt  : Exp TSEMI {$$=new NExp(*$1);}
      | CompSt {$$=new CompSt(*$1);}
      | TRETURN Exp TSEMI {$$=new NReturnStmt(*$2);}
      | TIF TLP Exp TRP Stmt {$$=new NIfStmt(*$3,*$5);}
      | TIF TLP Exp TRP Stmt TELSE Stmt {$$=new NIfStmt(*$3,*$5,*$7);}
      | TWHILE TLP Exp TRP Stmt {$$=new NWhileStmt(*$3,*$5);}
      ;

DefList:/*blank */ {$$=new DefList();} 
       | Def DefList {$2->push_back(*$1);}
       ;

Def  :Specifier DecList TSEMI {$$=new NDef(*$1,*$2);}
     ;

DecList : Dec {$$=new DecList();$$->push_back(*$1);}
        | Dec TCOMMA DecList {$3->push_back(*$1);}
        ;

Dec    : VarDec {$$=new NDec(*$1);}
       | VarDec TASSIGN Exp {$$=new NDec(*$1,*$3);}
       ;

Exp  : Exp TASSIGN Exp { $$ = new NAssignment(*$1, *$3); }
     | Exp TAND Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TOR Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TMUL Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TDIV Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TMINUS Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TPLUS Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TEQ Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TNE Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TLT Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TLE Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TGE Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | Exp TGT Exp {$$ = new NBinaryOperator(*$1, $2, *$3);}
     | TLP Exp TRP {$$=*$2;}
     | TMINUS Exp {$$=new NUnaryOperator(*$2,*$1);}
     | TNOT Exp {$$=new NUnaryOperator(*$2,*$1);}
     | TID TLP Args TRP {$$ = new NMethodCall(*$1, *$3);}
     | TID TLP TRP {$$ = new NMethodCall(*$1);}
     | Exp TLB Exp TRB {$$=new NArrayIndex(*$1,*$3);}
     | Exp TDOT TID {}
     | TID {$$=new NIdentifier(*$1);}
     | TINT {$$ = new NInteger(atol($1->c_str()));}
     | TFLOAT {$$ = new NDouble(atof($1->c_str()));}
     ;

Args : Exp {$$=new ExpList();}
     | Exp TCOMMA Args {$3->push_back(*$1);}
     ;

%%
