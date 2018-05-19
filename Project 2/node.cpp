#include <iostream>
#include <cstdio>
#include "node.h"
#include "parser.hpp"
//#include "token.hpp"
using namespace llvm;
static std::unique_ptr<Module> TheModule;
static LLVMContext* TheContext;
static IRBuilder<> *Builder;

void module_init(){
    TheContext=new LLVMContext();
    Builder=new IRBuilder<>(*TheContext);
    TheModule = llvm::make_unique<Module>("test the llvm",*TheContext);
    std::cout<<"init"<<std::endl;
}

NBlock::NBlock(int lineno,NExtDefList &llist):llist(&llist),lineno(lineno){
    
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
        std::cout<<"What the fuck?"<<std::endl;
        return;
    }
    GlobalVariable* ptr=TheModule->getGlobalVariable((spe.spe)->id->name);
    if(ptr==NULL){
        std::cout<<"Adding new variable "<<(spe.spe)->id->name<<std::endl;
        TheModule->getOrInsertGlobal((spe.spe)->id->name,Builder->getInt8Ty());
    }else{
        std::cout<<"Redefinition of variable"<<(spe.spe)->id->name<<std::endl;
    }
    return;
}

NExtDefNormal::NExtDefNormal(int lineno,const NSpecifier &spe,const NExtDecList& extlist):lineno(lineno),spe(&spe),extlist(extlist){
    auto type=(spe.type==TTYPE_INT)?Builder->getInt32Ty():Builder->getFloatTy();
    GlobalVariable* ptr;
    for(auto &var : extlist.vec){
        ptr=TheModule->getGlobalVariable(var->id->name);
        if(ptr==NULL){
            std::cout<<"Adding new variable "<<var->id->name<<std::endl;
            TheModule->getOrInsertGlobal(var->id->name,type);
        }else{
            std::cout<<"Redefinition of variable"<<var->id->name<<std::endl;
        }
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
    if(this->type!=0){
        print_w(i+2);
        std::printf("LP\n");
    }
    this->ptr->print(i+2);
    if(this->type!=0){
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

