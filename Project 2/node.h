#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <list>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;
extern int yylineno;
class NStmt;
class NExp;
class NSpecifier;
class NVarDec;
class NFuncDec;
class NStructSpecifier;
class NIdentifier;
class NDef;
class NParamDec;
class NCompSt;
class NExtDef;
class NDec;
class NStmtList;
class NDefList;
class NExpList;
class NDecList;
class NExtDecList;
class NVarList;

void module_init();

enum ExpType{
    EID=0x20,
    EARRAY=0x40,
    ERVAL=0x80,
    EINT=0x100,
    EFLOAT=0x200,
    ESTRUCT=0x400,
    EUSELESS=0x800
};

class ContextBlock{
public:
    std::unique_ptr<Module> curModule;
    LLVMContext*  curContext;
    ContextBlock(std::unique_ptr<Module> curModule, LLVMContext*  curContext):curContext(curContext)
    {
        this->curModule=std::move(curModule);
    }
};

class ContextStack{
public:
    std::vector<ContextBlock> contextstack;
    void pop();
    void push();
};

class Node {
public:
    virtual void print(int i) const;
};


class NExtDef : public Node{
public:
    int lineno;
	virtual void print(int i) const;
};

class NExtDefList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NExtDef*> vec;
    NExtDefList(int lineno):lineno(lineno){}
    NExtDefList(){}
    void push_front(NExtDef* ptr){
        vec.push_front(ptr);
    }
};

class NStmtList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NStmt*> vec;
    NStmtList(int lineno):lineno(lineno){}
    NStmtList(){}
    void push_front(NStmt* ptr){
        vec.push_front(ptr);
    }
};

class NVarList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NParamDec*> vec;
    NVarList(int lineno):lineno(lineno){}
    NVarList(){}
    void push_front(NParamDec* ptr);
};

class NExtDecList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NVarDec*> vec;
    NExtDecList(int lineno):lineno(lineno){}
    NExtDecList(){}
    void push_front(NVarDec* ptr){
        vec.push_front(ptr);
    }
};

class NDecList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NDec*> vec;
    NDecList(int lineno):lineno(lineno){}
    NDecList(){}
    void push_front(NDec* ptr){
        vec.push_front(ptr);
    }
};

class NDefList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NDef*> vec;
    NDefList(int lineno):lineno(lineno){}
    NDefList(){}
    void push_front(NDef* ptr){
        vec.push_front(ptr);
    }
};

class NExpList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NExp*> vec;
    NExpList(int lineno):lineno(lineno){}
    NExpList(){}
    void push_front(NExp* ptr){
        vec.push_front(ptr);
    }
};

class NBlock: public Node{
public:
    int lineno;
	virtual void print(int i) const;
    NExtDefList* llist;
    NBlock(int lineno,NExtDefList &llist);
};

class NExtDefNormal: public NExtDef{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    NExtDecList extlist;
    NExtDefNormal(int lineno,const NSpecifier &spe,const NExtDecList& extlist);
    NExtDefNormal(int lineno,const NSpecifier &spe);
};

class NExtDefFunc :public NExtDef{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    const NFuncDec* funcdef;
    const NCompSt* code;
    NExtDefFunc(int lineno,const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code);
};




class NStmt : public Node {
public:
    int lineno;
	virtual void print(int i) const;
    const NStmt* ptr;
    int type;
    NStmt(int lineno,const NStmt& ptr,int type):lineno(lineno),ptr(&ptr),type(type){}
    NStmt(int lineno,const NStmt& ptr):lineno(lineno),ptr(&ptr),type(0){}
    NStmt(int lineno):lineno(lineno),ptr(NULL){}
    NStmt():ptr(NULL){}
};

class NExp : public NStmt {
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* ptr;
    int type;
    NExp(int lineno,NExp& ptr,int type);
    NExp(int lineno,const NExp& ptr):lineno(lineno),ptr(&ptr),type(0){}
    NExp(int lineno):lineno(lineno),ptr(NULL){}
    NExp():ptr(NULL){}
};
class NIdentifier : public NExp{
public:
    int lineno;
    virtual void print(int i) const;
    std::string name;
    NIdentifier(int lineno,const std::string& name):lineno(lineno),name(name){}
    NIdentifier(const std::string& name):lineno(yylineno),name(name){}
};

class NStructSpecifier: public Node{
public:
    int lineno;
    int is_def;
    virtual void print(int i) const;
    const NIdentifier* id;
    NDefList defList;
    NStructSpecifier(int lineno,const NIdentifier& id,const NDefList& defList):lineno(lineno),id(&id),defList(defList),is_def(1){}
    NStructSpecifier(int lineno,const NIdentifier& id):lineno(lineno),id(&id),is_def(0){}
    NStructSpecifier(int lineno,const std::string & id_name):lineno(lineno),is_def(0){
        this->id=new NIdentifier(lineno,id_name);
    };
};


class NSpecifier: public Node{
public:
    int lineno;
    virtual void print(int i) const;
    int type;
    bool is_struct;
    NStructSpecifier* spe;

    NSpecifier(int lineno,int type):lineno(lineno),type(type),is_struct(false){}
    NSpecifier(int lineno,NStructSpecifier& spe):lineno(lineno),is_struct(true),spe(&spe){}
};

class NCompSt: public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    NStmtList klist;
    NDefList defList;
    NCompSt(int lineno);
    NCompSt(int lineno,const NCompSt& cmp){
        this->lineno=lineno;
        this->klist=cmp.klist;
        this->defList=cmp.defList;
    }
    void add(NDefList& dlist,NStmtList& slist);
};

class NDef :public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    NDecList dlist;
    NDef(int lineno,const NSpecifier& spe,const NDecList& dlist):lineno(lineno),spe(&spe),dlist(dlist){}
};

class NDec :public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NVarDec* vardec;
    const NExp*  expr;
    int is_assign;

    NDec(int lineno,const NVarDec& vardec):lineno(lineno),vardec(&vardec),is_assign(0){}
    NDec(int lineno,const NVarDec& vardec,const NExp&  expr):lineno(lineno),expr(&expr),is_assign(1),vardec(&vardec){}
};

class NReturnStmt: public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* res;
    NReturnStmt(int lineno,NExp& res):lineno(lineno),res(&res){}
};

class NIfStmt : public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* condition;
    const NStmt* ifstmt;
    const NStmt* elstmt;
    NIfStmt(int lineno,NExp& condition,NStmt& ifstmt):lineno(lineno),condition(&condition),ifstmt(&ifstmt),elstmt(NULL){}
    NIfStmt(int lineno,NExp& condition,NStmt& ifstmt,NStmt& elstmt):lineno(lineno),condition(&condition),ifstmt(&ifstmt),elstmt(&elstmt){}
};

class NWhileStmt :public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* condition;
    const NStmt* body;
    NWhileStmt(int lineno,NExp& condition,NStmt& body):lineno(lineno),condition(&condition),body(&body){}
};



class NVarDec : public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NIdentifier* id;
    NVarDec* next;
    int is_arr;
    int length;
    NVarDec(int lineno,const std::string& id):lineno(lineno),is_arr(0){       
        this->id=new NIdentifier(lineno,id);
        this->next=(NULL);
    }
    NVarDec(int lineno,NVarDec& next,int length):lineno(lineno),next(&next){        
    	this->is_arr=0;
        this->id=NULL;
        this->next->is_arr=1;
        this->length=length;
    }
};

class NParamDec:public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    const NVarDec* vardec;
    NParamDec(int lineno,const NSpecifier& spe,const NVarDec& vardec):lineno(lineno),spe(&spe),vardec(&vardec){}
};

class NFuncDec : public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NIdentifier* id;
    NVarList dlist;
    NFuncDec(int lineno,const NIdentifier& id):lineno(lineno),id(&id) {}
    NFuncDec(int lineno,const NIdentifier& id,NVarList& dlist):lineno(lineno),id(&id),dlist(dlist){}
};

class NInteger : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    long long value;
    char* endptr;
    NInteger(int lineno,std:: string& s):lineno(lineno){
        endptr=NULL;
        value=std::strtol(s.c_str(),&endptr,0);
        if(*endptr!='\0'){
            std::printf("Error type A at Line %d: \'Unknown Integer %s\'\n", yylineno, s.c_str());
            std::exit(1);
        }
    }
};

class NDouble : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    double value;
    NDouble(int lineno,double value) :lineno(lineno), value(value) { }
};

class NMethodCall : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    const NIdentifier* id;
    NExpList arguments;
    NMethodCall(int lineno,const NIdentifier& id, NExpList& arguments) :lineno(lineno),
        id(&id), arguments(arguments) { }
    NMethodCall(int lineno,const NIdentifier& id) : lineno(lineno),id(&id) { }
};

class NBinaryOperator : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    int op;
    const NExp* lhs;
    const NExp* rhs;
    NBinaryOperator(int lineno,NExp& lhs, int op, NExp& rhs);
};

class NUnaryOperator : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    int op;
    const NExp* rhs;
    NUnaryOperator(int lineno,NExp& rhs, int op);
};

class NArrayIndex :public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* index;
    const NExp* arr;
    NArrayIndex(int lineno,NExp& arr,NExp& index);
};

class NAssignment : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* lhs;
    const NExp* rhs;
    NAssignment(int lineno,NExp& lhs, NExp& rhs);
};

class NStructMem :public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* expr;
    const NIdentifier* member;
    NStructMem(int lineno,const NExp& expr,const std::string member):lineno(lineno),expr(&expr){
        this->member=new NIdentifier(lineno,member);
        //this->type=EID;
    }
};
