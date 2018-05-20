#include <iostream>
#include <cstdio>
#include "node.h"
#include "parser.hpp"
//#include "token.hpp"
using namespace llvm;
static std::unique_ptr<Module> TheModule,CurModule;
static LLVMContext* TheContext , *CurContext;
static IRBuilder<> *Builder;
static ContextStack contextstack;

void ContextStack::pop(){
    delete CurContext;
    CurContext=NULL;
    if(!contextstack.empty()){
        CurContext=contextstack.rbegin()->curContext;
        CurModule=std::move(contextstack.rbegin()->curModule);
        contextstack.pop_back();
    }else{
        CurModule=NULL;
    }
}

Type* lookforname(std::string name){
    if(CurContext!=NULL){
        auto k=CurModule->getGlobalVariable(name);
        if(k!=NULL){
            return k->getValueType();
        }
    }
    auto k=TheModule->getGlobalVariable(name);
    if(k==NULL)return NULL;
    else return k->getValueType();
}

void ContextStack::push(){
    contextstack.push_back(ContextBlock(std::move(CurModule),CurContext));
    CurContext=new LLVMContext();
    CurModule=llvm::make_unique<Module>("Enter function",*CurContext);
}

void module_init(){
    TheContext=new LLVMContext();
    CurContext=NULL;
    Builder=new IRBuilder<>(*TheContext);
    TheModule = llvm::make_unique<Module>("Global Context",*TheContext);
//    std::cout<<"init"<<std::endl;
}

NBlock::NBlock(int lineno,NExtDefList &llist):llist(&llist),lineno(lineno){
    
}

static void err_info(int type,int lineno,const char* msg,const char* more){
    printf("Error type %3d at Line %4d: %s ",type,lineno,msg);
    puts(more);
}

void Node::print(int i) const{
    return;
};

void NExtDef::print(int i) const{
    return;
}

void print_w(int i){
    for(int j=0;j<i;j++){
        std::cout<<" ";
    }
}

void print_linno (int lineno,const char* s){
    std::printf("%s (%d)\n",s,lineno);
}
void NExtDefNormal::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"ExtDef");
    this->spe->print(i+2);
    for(auto iter=this->extlist.vec.begin();iter!=this->extlist.vec.end();iter++){
        (*iter)->print(i+2);
    }
    print_w(i+2);
    std::printf("SEMI\n");
}

void NExtDefList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"ExtDefList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
    }
}

NExtDefNormal::NExtDefNormal(int lineno,const NSpecifier &spe):lineno(lineno),spe(&spe){
    if(spe.is_struct==false){
        std::cout<<"Parsing Struct Error?"<<std::endl;
        exit(-1);
        return;
    }
    auto ptr=lookforname((spe.spe)->id->name);
    if(ptr==NULL){
        //vector<Type*> members;
        TheModule->getOrInsertGlobal((spe.spe)->id->name,Builder->getInt8Ty());
    }else{
        //std::cout<<"Redefinition of variable"<<(spe.spe)->id->name<<std::endl;
        err_info(3,lineno,"Redefined of structure",(spe.spe)->id->name.c_str());
    }
    return;
}

NExtDefNormal::NExtDefNormal(int lineno,const NSpecifier &spe,const NExtDecList& extlist):lineno(lineno),spe(&spe),extlist(extlist){
    auto type=(spe.type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
    GlobalVariable* ptr;
    for(auto &var : extlist.vec){
        std::string tmpname;
        int total=1;
        if(var->next==NULL){
            tmpname=var->id->name;
        }else{
            auto arrptr=var->next;
            while(arrptr->next!=NULL){
                total*=arrptr->length;
                arrptr=arrptr->next;
            }
            tmpname=arrptr->id->name;
        }
        ptr=TheModule->getGlobalVariable(tmpname);
        if(ptr==NULL){
            if(var->next==NULL)
                TheModule->getOrInsertGlobal(tmpname,type);
            else{
                TheModule->getOrInsertGlobal(tmpname,ArrayType::get(type,total));
            }
        }else{
            //std::cout<<"Redefinition of variable"<<var->id->name<<std::endl;
            err_info(3,lineno,"Redefined of variable",tmpname.c_str());
        }
    }
}

void NVarList::push_front(NParamDec* ptr){
     auto name1=ptr->vardec->id->name;
     for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        if(name1==(*iter)->vardec->id->name){
            err_info(18,this->lineno,"Function args have collision name",name1.c_str());
        }
     }
     this->vec.push_front(ptr);
}

NExtDefFunc::NExtDefFunc(int lineno,const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code):lineno(lineno),spe(&spe),funcdef(&funcdef),code(&code){
    std::vector<Type*> args;
    if(TheModule->getFunction(funcdef.id->name)!=NULL){
        err_info(4,lineno,"Redefined of function",funcdef.id->name.c_str());
    }else{
        auto rettype=(spe.type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
        for(auto &tmp: funcdef.dlist.vec){
            if(tmp->spe->is_struct){
                err_info(19,lineno,"Not supported struct args yet","");
            }
            auto tmptype=(tmp->spe->type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
            args.push_back(tmptype);
        }
        FunctionType *FT =FunctionType::get(rettype, args, false);
        TheModule->getOrInsertFunction(funcdef.id->name,FT);
    }
}
void NExtDecList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"ExtDecList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
    }
}

void NStmtList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"StmtList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
    }
}

void NDecList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"DecList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
    }
}

void NVarList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"VarList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
        if((++iter)!=this->vec.end()){
            print_w(i+2);
            std::printf("COMMA\n");
            --iter;
        }
    }
}

void NDefList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"DefList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
    }
}

void NExpList::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Args");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        (*iter)->print(i+2);
        if((++iter)!=this->vec.end()){
            print_w(i+2);
            std::printf("COMMA\n");
            iter--;
        }
    }
}
NExp::NExp(int lineno,NExp& pp,int type):lineno(lineno),ptr(&pp),type(type){
    int tmptype=-1;
    if(type&EID){
        NIdentifier* ptr=dynamic_cast<NIdentifier*>(&pp);
        auto ttype=lookforname(ptr->name);
        if(ttype==NULL){
            err_info(1,this->lineno,"Use of undefine variable",ptr->name.c_str());
        }else{
            if(ttype->isIntegerTy()){
                tmptype=EINT;
            }else if(ttype->isFloatTy()){
                tmptype=EFLOAT;
            }else if(ttype->isArrayTy()){
                tmptype=EARRAY;
                this->type^=EID;
            }
        }
    }
    if(tmptype!=-1){
        this->type|=tmptype;
    }
}
NAssignment::NAssignment(int lineno,NExp& lhs, NExp& rhs) : lineno(lineno),lhs(&lhs), rhs(&rhs){
    if(!(lhs.type|ERVAL)){
        err_info(6,this->lineno,"Rval appears on the right","");
    }
    int tmptype=-1;
    this->type=lhs.type;
    if(lhs.type&EID){
        tmptype=(this->type&EINT)?EINT:EFLOAT;
    }else if(lhs.type&ESTRUCT){
        tmptype=(this->type&EINT)?EINT:EFLOAT;
    }else if(lhs.type&EARRAY){
        tmptype=(this->type&EINT)?EINT:EFLOAT;
    }
    //printf("Type1 %x, Type 2 %x\n",lhs.type,rhs.type);
    if((tmptype!=-1)&&(!(tmptype&rhs.type))){
        err_info(5,this->lineno,"Assignment type mismatched","");
    }
    this->type=tmptype;
}

void NBlock::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Program");
    this->llist->print(i+2);
}

void NExtDefFunc::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"ExtDef");
    this->spe->print(i+2);
    this->funcdef->print(i+2);
    this->code->print(i+2);
}

void NStructSpecifier::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"StructSpecifier");
    print_w(i+2);
    std::printf("STRUCT\n");
    if(this->defList.vec.size()!=0){
        print_w(i+2);
        std::printf("OptTag\n");
        this->id->print(i+4);
        print_w(i+2);
        std::printf("LC\n");
        this->defList.print(i+2);
        print_w(i+2);
        std::printf("RC\n");
    }else{
        print_w(i+2);
        std::printf("Tag\n");
        this->id->print(i+4);
    }
}

void NSpecifier::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Specifier");
    if(this->is_struct!=0){
        this->spe->print(i+2);
    }else{
        print_w(i+2);
        std::printf("TYPE\n");
    }
}

void NStmt::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Stmt");
    this->ptr->print(i+2);
    if(this->type!=0){
        print_w(i+2);
        std::printf("SEMI\n");
    }
}

void NExp::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Exp");
    if(this->type&EUSELESS){
        print_w(i+2);
        std::printf("LP\n");
    }
    this->ptr->print(i+2);
    if(this->type&EUSELESS){
        print_w(i+2);
        std::printf("RP\n");
    }
}

void NCompSt::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"CompSt");
    print_w(i+2);
    std::printf("LC\n");
    this->defList.print(i+2);
    this->klist.print(i+2);
    print_w(i+2);
    std::printf("RC\n");
}

void NDef::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Def");
    this->spe->print(i+2);
    this->dlist.print(i+2);
    print_w(i+2);
    std::printf("SEMI\n");
}

void NDec::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"Dec");
    this->vardec->print(i+2);
    if(is_assign==1){
        print_w(i);
        std::printf("ASSIGNOP\n");
        this->expr->print(i+2);
    }
}

void NReturnStmt::print(int i) const{
    print_w(i);
    std::printf("RETURN\n");
    this->res->print(i);
    print_w(i);
    std::printf("SEMI\n");
}

void NIfStmt::print(int i) const{
    print_w(i);
    std::printf("IF\n");
    print_w(i);
    std::printf("LP\n");
    this->condition->print(i);
    print_w(i);
    std::printf("RP\n");
    this->ifstmt->print(i);
    if(this->elstmt!=NULL){
        print_w(i);
        std::printf("ELSE\n");
        this->elstmt->print(i);
    }
}

void NWhileStmt::print(int i) const{
    print_w(i);
    std::printf("WHILE\n");
    print_w(i);
    std::printf("LP\n");
    this->condition->print(i+2);
    print_w(i);
    std::printf("RP\n");
    this->body->print(i+2);
}

void NIdentifier::print(int i) const{
    print_w(i);
    std::printf("ID : %s\n",this->name.c_str());
}

void NVarDec::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"VarDec");
    if(this->next==NULL){
        this->id->print(i+2);
    }else{
        this->next->print(i+2);
        print_w(i+2);
        std::printf("LB\n");
        print_w(i+2);
        std::printf("INT : %d\n",this->length);
        print_w(i+2);
        std::printf("RB\n");
    }
}

void NParamDec::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"ParamDec");
    this->spe->print(i+2);
    this->vardec->print(i+2);
}

void NFuncDec::print(int i) const{
    print_w(i);
    print_linno(this->lineno,"FuncDec");
    this->id->print(i+2);
    print_w(i+2);
    std::printf("LP\n");
    if(!this->dlist.vec.empty())
        this->dlist.print(i+2);
    print_w(i+2);
    std::printf("RP\n");
}

void NInteger::print(int i) const{
    print_w(i);
    std::printf("INT : %lld\n",this->value);
}

void NDouble::print(int i) const{
    print_w(i);
    std::printf("FLOAT : %f\n",this->value);
}

void NMethodCall::print(int i) const{
    this->id->print(i);
    print_w(i);
    std::printf("LP\n");
    this->arguments.print(i);
    print_w(i);
    std::printf("RP\n");
}

void NBinaryOperator::print(int i) const{
    this->lhs->print(i);   
    print_w(i);
    switch(this->op){
    case TEQ:
    case TNE:
    case TGE:
    case TGT:
    case TLE:
    case TLT:
        std::printf("RELOP\n");
        break;
    case TMINUS:
        std::printf("MINUS\n");
        break;
    case TPLUS:
        std::printf("PLUS\n");
        break;
    case TDIV:
        std::printf("DIV\n");
        break;
    case TMUL:
        std::printf("MUL\n");
        break;
    case TAND:
        std::printf("AND\n");
        break;
    case TOR:
        std::printf("OR\n");
        break;
    }
    this->rhs->print(i);
}

void NUnaryOperator::print(int i) const{
    print_w(i);
    switch(this->op){
    case TMINUS:
        std::printf("MINUS\n");
        break;
    case TNOT:
        std::printf("NOT\n");
        break;
    }
    this->rhs->print(i);
}

void NArrayIndex::print(int i) const{
    this->arr->print(i);
    print_w(i);
    std::printf("LB\n");
    this->index->print(i);
    print_w(i);
    std::printf("RB\n");
}

void NAssignment::print(int i)const {
    this->lhs->print(i);
    print_w(i);
    std::printf("ASSIGNOP\n");
    this->rhs->print(i);
}

void NStructMem::print(int i)const{
    this->expr->print(i);
    print_w(i);
    std::printf("DOT\n");
    this->member->print(i);
}

NArrayIndex::NArrayIndex(int lineno,NExp& arr,NExp& index):lineno(lineno),index(&index),arr(&arr){
    if(arr.type&EID){
        err_info(10,this->lineno,"Indexing a non-array variable","");
    }else if(arr.type&EARRAY){
        this->type|=EARRAY;
    }
    this->type|=arr.type&(EINT|EFLOAT);
    if((index.type&EINT)==0){
        err_info(12,this->lineno,"Non-integer array index","");
    }
}

NUnaryOperator::NUnaryOperator(int lineno,NExp& rhs, int op) :lineno(lineno),op(op),rhs(&rhs){ 
    if(rhs.type&(EINT|EFLOAT)!=0){
        err_info(7,this->lineno,"Operand and operator's type mismatch","");
    }
    this->type=rhs.type&(EINT|EFLOAT);
}

NBinaryOperator::NBinaryOperator(int lineno,NExp& lhs, int op, NExp& rhs) :lineno(lineno),op(op),lhs(&lhs), rhs(&rhs){
    if((rhs.type&(EINT|EFLOAT))!=(lhs.type&(EINT|EFLOAT))){
        err_info(7,this->lineno,"Operands' type mismatch","");
    }
    this->type=rhs.type&(EINT|EFLOAT);
}