#include <iostream>
#include <cstdio>
#include "node.h"
#include "parser.hpp"
//#include "token.hpp"

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
    std::printf("INT : %f\n",this->value);
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
