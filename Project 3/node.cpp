#include <iostream>
#include <cstdio>
#include <map>
#include "node.h"
#include "parser.hpp"
//#include "token.hpp"

using namespace llvm;
static std::unique_ptr<Module> TheModule,CurModule;
static LLVMContext* TheContext , *CurContext;
static IRBuilder<> *Builder;
static ContextStack contextstack;
static std::map<std::string,std::vector<std::string> > stable;

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

Type* createStructType(std::string name,std::map<std::string,Type*>& ivec){
    std::vector<Type*> members;
    stable[name];
    for(auto &k:ivec){
        stable[name].push_back(k.first);
        members.push_back(k.second);
    }
    auto s=StructType::create(*TheContext);
    s->setName(name);
    s->setBody(members);
    return s;
}

Type* gettype(const NSpecifier &spe){
    if(!spe.is_struct){
        return spe.type==TTYPE_INT?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
    }
    return NULL;
}

Type* defineStructure(const NSpecifier &spe){
    auto lineno=spe.lineno;
    auto ptr=lookforname((spe.spe)->id->name);
    std::map<std::string,Type*> ivec;
    if(ptr==NULL){       
        auto structptr=spe.spe;
        for(auto &p1:structptr->defList.vec){
            auto tmptype=gettype(*(p1->spe));
            if(tmptype==NULL){  //if structure
                auto sptr=defineStructure(*(p1->spe));
                if(sptr!=NULL){
                    if(ivec.find(p1->spe->spe->id->name)==ivec.end()){
                        ivec[p1->spe->spe->id->name]=sptr;
                    }
                }
            }else{
                for(auto &p2:p1->dlist.vec){  //if normal var or array;
                    if(p2->is_assign){
                        err_info(15,p2->lineno,"Initialization in definition of stucture","");
                        return NULL;
                    }else{
                        auto p3=p2->vardec;
                        if(p3->next==NULL){
                            if(ivec.find(p3->id->name)==ivec.end()){
                                ivec[p3->id->name]=tmptype;
                            }
                            else{
                                err_info(15,p2->lineno,"Collision of member definition","");
                                return NULL;
                            }
                        }else{
                            int total=1;
                            auto arrptr=p3->next;
                            while(arrptr->next!=NULL){
                                total*=arrptr->length;
                                arrptr=arrptr->next;
                            }
                            if(ivec.find(arrptr->id->name)==ivec.end()){
                                ivec[arrptr->id->name]=ArrayType::get(tmptype,total);
                            }
                            else{
                                err_info(15,p2->lineno,"Collision of member definition","");
                                return NULL;
                            }  
                        }
                    }
                }
            }
        }
        auto res=createStructType((spe.spe)->id->name,ivec);
        TheModule->getOrInsertGlobal((spe.spe)->id->name,res);
        return res;
    }else{
        err_info(3,lineno,"Redefined of structure",(spe.spe)->id->name.c_str());
        return NULL;
    }
    return NULL;
}

NExtDefNormal::NExtDefNormal(int lineno,const NSpecifier &spe):lineno(lineno),spe(&spe){
    if(spe.is_struct){
        if(spe.spe->is_def){
            defineStructure(spe);
        }      
    }   
}

bool checkStructHasDef(const NSpecifier &spe){
    if(!spe.is_struct)return false;
    return stable.find(spe.spe->id->name)!=stable.end();
}

bool createVar(const NSpecifier &spe,const NVarDec &var){
    auto type=gettype(spe);
    std::string name;
    int total=1;
    bool is_arr=true;
    auto next=var.next;
    if(next==NULL){
        name=var.id->name;
        is_arr=false;
    }else{
        while(next->next!=NULL){
            total*=var.length;
            next=next->next;
        }
        name=next->id->name;
    }
    if(type==NULL){
        if(checkStructHasDef(spe)==false){
            err_info(17,spe.lineno,"Structure is not defined","");
            return false;
        }else{
            type=lookforname(spe.spe->id->name);
        }
    }
    if(lookforname(name)!=NULL){
        err_info(3,spe.lineno,"Redefined of variable",name.c_str());
    }
    //printf("Creating var %s, is struct %d\n",name.c_str(),type->isStructTy());
    TheModule->getOrInsertGlobal(name,is_arr?ArrayType::get(type,total):type);
    return true;
}

NExtDefNormal::NExtDefNormal(int lineno,const NSpecifier &spe,const NExtDecList& extlist):lineno(lineno),spe(&spe),extlist(extlist){
    auto type=(spe.type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
    GlobalVariable* ptr;
    for(auto &var : extlist.vec){
        createVar(spe,*var);
    }
}

NDef::NDef(int lineno,const NSpecifier& spe,const NDecList& dlist):lineno(lineno),spe(&spe),dlist(dlist){
    for(auto &var:dlist.vec){
        createVar(spe,*(var->vardec));
    }
}
NCompSt::NCompSt(int lineno):lineno(lineno){
}

void NMethodCall::check(){
    auto ttype=TheModule->getFunction(id->name);
    bool flag=true;
    //puts("Hello?");
    if(ttype==NULL){
        if(lookforname(id->name)){
            err_info(11,this->lineno,"Calling a non-callable variable",id->name.c_str());
            return;
        }
        err_info(2,this->lineno,"Use of undefined function",id->name.c_str());
        return;
    }
    int i=0;
    auto tttype=ttype->getFunctionType();
    if(arguments.vec.size()==tttype->getNumParams()){
        //std::cout<<arguments.vec.size()<<std::endl;
        auto args=tttype->params();
        for(auto iter=arguments.vec.begin();iter!=arguments.vec.end();iter++,i++){
            if((((*iter)->type&EINT)&&args[i]->isIntegerTy())||(((*iter)->type&EFLOAT)&&args[i]->isFloatTy())){
            }else{
                flag=false;
                break;
            }
        }
    }else{
        flag=false;
    }
    if(flag==false){
        err_info(9,this->lineno,"Not matching function arguments",id->name.c_str());
    }
}
NMethodCall::NMethodCall(int lineno,const NIdentifier& id) : lineno(lineno),id(&id){
    arguments.vec.clear();
    check();
}

void NCompSt::add(NDefList& dlist,NStmtList& slist){
    klist=slist;
    defList=dlist;
}

void NVarList::push_front(NParamDec* ptr){
    auto name1=ptr->vardec->id->name;
    for(auto iter=this->vec.begin();iter!=this->vec.end();iter++){
        if(name1==(*iter)->vardec->id->name){
            err_info(18,this->lineno,"Function args have collision name",name1.c_str());
        }
    }
    this->vec.push_front(ptr);
    createVar(*(ptr->spe),*(ptr->vardec));
}


NExtDefFunc::NExtDefFunc(int lineno,const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code):lineno(lineno),spe(&spe),funcdef(&funcdef),code(&code){
    std::vector<Type*> args;
    if(TheModule->getFunction(funcdef.id->name)!=NULL){
        err_info(4,lineno,"Redefined of function",funcdef.id->name.c_str());
    }else{
        auto rettype=(spe.type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
        for(auto &retcheck:code.klist.vec){
            if(retcheck->type==2){
                auto tptr=dynamic_cast<const NReturnStmt*>(retcheck->ptr);
                auto curtype=tptr->res->type;
                //printf("Curtype %x\n",curtype);
                /*
                if(tptr->res->type&EID){
                    auto ttp=dynamic_cast<const NIdentifier*>(tptr->res);
                    getchar();
                    auto re=lookforname(ttp->name);
                    getchar();
                    if(re!=NULL){
                        if(re->isIntegerTy())curtype|=EINT;
                        if(re->isFloatTy())curtype|=EFLOAT;
                    }
                }
                */
                if((curtype&EINT)&&rettype->isIntegerTy()||(curtype&EFLOAT)&&rettype->isFloatTy()){

                }else{
                    err_info(8,this->lineno,"type of return value mismatched","");
                }
            }
        }
        for(auto &tmp: funcdef.dlist.vec){
            if(tmp->spe->is_struct){
                err_info(19,lineno,"Not supported struct args yet","");
                return;
            }
            auto tmptype=(tmp->spe->type==TTYPE_INT)?Type::getInt32Ty(*TheContext):Type::getFloatTy(*TheContext);
            //TheModule->getOrInsertGlobal(tmp->vardec->id->name,tmptype);
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
        print_w(i+2);
        std::printf("COMMA\n");
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
        if(true){
            print_w(i+2);
            std::printf("COMMA\n");
            //iter--;
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
            }else if(ttype->isStructTy()){
                tmptype=ESTRUCT;
                this->structname=ptr->name;
                this->type^=EID;
            }
        }
    }
    //printf("Tmpytype %x\n",tmptype);
    if(tmptype!=-1){
        this->type|=tmptype;
    }
}
NAssignment::NAssignment(int lineno,NExp& lhs, NExp& rhs) : lineno(lineno),lhs(&lhs), rhs(&rhs){
    if(!(lhs.type&ERVAL)){
        err_info(6,this->lineno,"Rval appears on the right","");
    }
    int tmptype=-1;
    this->type=lhs.type;
    tmptype=(this->type&EINT)?EINT:EFLOAT;
    //printf("Type1 %x, Type 2 %x, tmp type %x\n",lhs.type,rhs.type,tmptype);
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
    if(this->type!=1){
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

NStructMem::NStructMem(int lineno,const NExp& expr,const std::string member):lineno(lineno),expr(&expr){
        this->member=new NIdentifier(lineno,member);
        if(!(expr.type&ESTRUCT)){
            err_info(13,this->lineno,"Getting member of a non-struct variable","");
        }else{
            if(stable.find(expr.structname)!=stable.end()){
                err_info(21,this->lineno,"Getting member of a struct definition",expr.structname.c_str());
            }else{
                auto tmptype=lookforname(expr.structname);
                std::string thename=tmptype->getStructName();
                auto ttype=TheModule->getTypeByName(thename);
                auto vartype=lookforname(member);
                auto structname=ttype->getName();
                unsigned int index=0;
                for(index=0;index<stable[structname].size();index++){
                    if(stable[structname][index]==member)break;
                }
                if(vartype==NULL||index==stable[structname].size()){
                    err_info(13,this->lineno,"Getting a non-existing member",member.c_str());
                }else{                    
                    auto tmptype=ttype->getElementType(index);
                    if(tmptype->isIntegerTy()){
                        this->type|=EINT;
                        this->type|=EID;
                    }else if(tmptype->isFloatTy()){
                        this->type|=EFLOAT;
                        this->type|=EID;
                    }else if(tmptype->isStructTy()){
                        this->type|=ESTRUCT;
                        this->structname=member;
                    }else if(tmptype->isArrayTy()){
                        this->type|=EARRAY;
                    }
                }
            }
        }
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
