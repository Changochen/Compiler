%{
    #include "node.h"
    #include <cstdio>
    #include <cstdlib>
    NBlock *programBlock; /* the top level root node of our final AST */

extern int yylex();
    void yyerror(const char *s) { std::printf("Error: %s\n", s);std::exit(1); }
%}

/* Represents the many different ways we can access our data */
%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NDefinition* *def;
    NFuncDefinition* *funcdef;
    NParameter *para;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::vector<NParameter*> *paravec;
    std::vector<NDefinition*> *defs;
    std::vector<NStatement*> *stmts;
    std::string *string;
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

%type <ident> ident optarg tag 
%type <expr>  Exp Dec 
%type <varvec> VarList
%type <var_decl> VarDec
%type <para> ParamDec
%type <funcdef> FunDec
%type <paravec> VarList
%type <exprvec> Args DecList 
%type <block> 
%type <def> Def
%type <defs> DefList
%type <stmt> Stmt
%type <stmts> StmtList CompSt
%type <token> comparison logic mathop

/* Operator precedence for mathematical operators */
//%left TAND TOR
//%left TEQ TNE TLT TLE TGT TGE
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

Program : ExtDefList { programBlock = $1; }
        ;

ExtDefList:/* */{}
          : ExtDef {}
          : ExtDefList ExtDef {}
          ;

ExtDef    : Specifier ExtDecList TSEMI {}
          : Specifier TSEMI {}
          : Specifier FunDec CompSt {}
          ;

ExtDecList: VarDec {}
          : ExtDecList COMMA VarDec {}
          ;

Specifier : TTYPE_INT {}
          | TTYPE_FLOAT {}
          | StructSpecifier {}
          ;


StructSpecifier:TSTRUCT OptTag LC DefList RC { $$=new NStructure(*$2,*$4);}
          | TSTRUCT Ident {$$=new NStructure(*$2);}
          ;

OptTag : /* blank */ {}
          | Ident { $$ = new NIdentifier(*$1);}
          ;

VarDec : TID {}
       | VarDec TLB TINT TRB {}
       ;

FunDec : TID TLP VarList TRP {}
       | TID TLP TRP {}
       ;

VarList :ParamDec TCOMMA VarList {}
       |ParamDec {}
       ;

ParamDec: Specifier VarDec {}
        ;

CompSt : TLC DefList StmtList TRC {}
       ;

StmtList :/* blank */ {}
         | Stmt {}
         | StmtList Stmt {}
         ;

Stmt  : Exp TSEMI {}
      | CompSt {}
      | TRETURN Exp TSEMI {}
      | TIF TLP Exp TRP Stmt {}
      | TIF TLP Exp TRP Stmt TELSE Stmt {}
      | TWHILE TLP Exp TRP Stmt {}
      ;

DefList:/*blank */ {} 
       | Def {}
       | DefList Def {}
       ;

Def  :Specifier DecList TSEMI {}
     ;

DecList : Dec {}
        | DecList TCOMMA Dec {}
        ;

Dec    : VarDec {}
       | Exp TASSIGN VarDec {}
       ;

Exp :  Exp TASSIGN Exp { $$ = new NAssignment(*$<ident>1, *$3); }
     | Exp logic Exp {}
     | Exp mathop Exp  {}
     | Exp comparison Exp {}
     | TLP Exp RLP {}
     | TMINUS Exp {}
     | TNOT Exp {}
     | TID TLP Args TRP {}
     | TID TLP TRP {}
     | Exp TLB Exp TRB {}
     | Exp TDOT TID {}
     | TID {}
     | TINT {}
     | TFLOAT {}
     ;

Args : Exp {}
     | Args COMMA Exp {}
     ;

comparison : TEQ | TNE | TLT | TLE | TGT | TGE;
mathop :TMUL | TDIV | TPLUS | TMINUS;
logic : TAND | TOR;

%%
