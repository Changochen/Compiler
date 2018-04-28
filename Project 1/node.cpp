#include <iostream>
#include <cstdio>
#include "node.h"

void NExtDef::print(int);
void NExtDefNormal::print(int);
void NExtDefList::print(int);
void NStmtList::print(int);
void NVarList::print(int);
void NVarDecList::print(int);
void NDecList::print(int);
void NDefList::print(int);
void NExpList::print(int);
void NBlock::print(int);
void NExtDefFunc::print(int);
void NStructSpecifier::print(int);
void NSpecifier::print(int);
void NStmt::print(int);
void NExp::print(int);
void NCompSt::print(int);
void NDef::print(int);
void NDec::print(int);
void NReturnStmt::print(int);
void NIfStmt::print(int);
void NWhileStmt::print(int);
void NIdentifier::print(int);
void NVarDec::print(int);
void NParamDec::print(int);
void NFuncDec::print(int);
void NInteger::print(int);
void NDouble::print(int);
void NMethodCall::print(int);
void NBinaryOperator::print(int);
void NUnaryOperator::print(int);
void NArrayIndex::print(int);
void NAssignment::print(int);
void NStrutMem::print(int);

void NExtDef::print(int i){
    return;
}

void print_w(int i){
    for(int j=0;j<i;j++){
        std::cout<<" ";
    }
}

void print_linno(const char* s){
    std::printf("%s (%d)\n",s,yylineno);
}
void NExtDefNormal::print(int i){
    print_w(i);
    print_linno("ExtDef");
    this->spe->print(i+2);
    for(auto iter=this->extlist.vec.begin();iter!=this->extlist.vec.end();iter++){
        iter->print(i+2);
    }
    print_w(i+2);
    std::printf("SEMI\n");
}

void NExtDefList::print(int i){
    print_w(i);
    print_linno("ExtDefList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        iter->print(i+2);
    }
}

void NStmtList::print(int i){
    print_w(i);
    print_linno("StmtList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        iter->print(i+2);
    }
}

void NDecList::print(int i){
    print_w(i);
    print_linno("DecList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        iter->print(i+2);
    }
}

void NDefList::print(int i){
    print_w(i);
    print_linno("DefList");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        iter->print(i+2);
    }
}

void NDefList::print(int i){
    print_w(i);
    print_linno("Args");
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        iter->print(i+2);
        if((++iter)!=this->vec.end()){
            print_w(i+2);
            std::printf("COMMA\n");
            iter--;
        }
    }
}

void NBlock::print(int i){
    print_w(i);
    print_linno("Program");
    this->llist.print(i+2);
}

void NExtDefFunc::print(i){
    print_w(i);
    print_linno("ExtDef");
    this->spe->print(i+2);
    this->funcdef->print(i+2);
    this->code->print(i+2);
}

void NStructSpecifier::print(int i){
    print_w(i);
    print_linno("StructSpecifier");
    print_w(i+2);
    std::printf("STRUCT");
    if(this->defList.vec.size()!=0){
        print_w(i+2);
        std::printf("OptTag");
        this->id->print(i+4);
        print_w(i+2);
        std::printf("LC");
        this->defList.print(i+2);
        print_w(i+2);
        std::printf("RC");
    }else{
        print_w(i+2);
        std::printf("Tag");
        this->id->print(i+4);
    }
}

void NSpecifier::print(int i){
    print_w(i);
    print_linno("Specifier");
    if(this->is_struct!=0){
        this->spe->print(i+2);
    }else{
        print_w(i+2);
        print_linno("TYPE");
    }
}

void 
