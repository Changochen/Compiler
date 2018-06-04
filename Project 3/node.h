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
#include "llvm/IR/NoFolder.h"

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

enum StmtType{
    SCOMPST=0,
    SNORMAL,
    SRETURN,
    SIF,
    SIFELSE,
    SWHILE,
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
    virtual Value *codegen() = 0;
    Value* codeval=NULL;
};


class NExtDef : public Node{
public:
    int lineno;
	virtual void print(int i) const;
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen();
};

class NVarList:public Node{
public:
    int lineno;
	virtual void print(int i) const;
    std::list<NParamDec*> vec;
    NVarList(int lineno):lineno(lineno){}
    NVarList(){}
    void push_front(NParamDec* ptr);
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
};

class NBlock: public Node{
public:
    int lineno;
	virtual void print(int i) const;
    NExtDefList* llist;
    NBlock(int lineno,NExtDefList &llist);
    virtual Value *codegen(){
        return NULL;
    };
};

class NExtDefNormal: public NExtDef{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    NExtDecList extlist;
    NExtDefNormal(int lineno,const NSpecifier &spe,const NExtDecList& extlist);
    NExtDefNormal(int lineno,const NSpecifier &spe);
    virtual Value *codegen(){
        return NULL;
    };
};

class NExtDefFunc :public NExtDef{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    const NFuncDec* funcdef;
    const NCompSt* code;
    NExtDefFunc(int lineno,const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code);
    virtual Value *codegen(){
        return NULL;
    };
};

class NStmt : public Node {
public:
    int lineno;
	virtual void print(int i) const;
    NStmt* ptr;
    int type;
    NStmt(int lineno,NStmt& ptr,int type):lineno(lineno),ptr(&ptr),type(type){}
    NStmt(int lineno,NStmt& ptr):lineno(lineno),ptr(&ptr),type(0){}
    NStmt(int lineno):lineno(lineno),ptr(NULL){}
    NStmt():ptr(NULL){}
    virtual Value *codegen(){
        if(ptr!=NULL){
            ptr->codegen();
        }
        return NULL;
    };
};

class NExp : public NStmt {
public:
    int lineno;
    std::string structname;
	virtual void print(int i) const;
    NExp* ptr;
    int type;
    NExp(int lineno,NExp& ptr,int type);
    NExp(int lineno,NExp& ptr):lineno(lineno),ptr(&ptr),type(0){}
    NExp(int lineno):lineno(lineno),ptr(NULL){}
    NExp():ptr(NULL){}
    virtual Value *codegen();
};
class NIdentifier : public NExp{
public:
    int lineno;
    virtual void print(int i) const;
    std::string name;
    NIdentifier(int lineno,const std::string& name):lineno(lineno),name(name){}
    NIdentifier(const std::string& name):lineno(yylineno),name(name){}
    virtual Value *codegen();
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
    virtual Value *codegen(){
        return NULL;
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
    virtual Value *codegen(){
        return NULL;
    };
};

class NCompSt: public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    NStmtList klist;
    NDefList defList;
    NCompSt(int lineno);
    NCompSt(int lineno,NCompSt& cmp){
        this->lineno=lineno;
        this->klist=cmp.klist;
        this->defList=cmp.defList;
    }
    void add(NDefList& dlist,NStmtList& slist);
    virtual Value *codegen();
};

class NDef :public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    NDecList dlist;
    NDef(int lineno,const NSpecifier& spe,const NDecList& dlist);
    virtual Value *codegen(){
        return NULL;
    };
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
    virtual Value *codegen(){
        return NULL;
    };
};

class NReturnStmt: public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    NExp* res;
    NReturnStmt(int lineno,NExp& res);
    virtual Value *codegen();
};

class NIfStmt : public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    NExp* condition;
    NStmt* ifstmt;
    NStmt* elstmt;
    NIfStmt(int lineno,NExp& condition,NStmt& ifstmt):lineno(lineno),condition(&condition),ifstmt(&ifstmt),elstmt(NULL){}
    NIfStmt(int lineno,NExp& condition,NStmt& ifstmt,NStmt& elstmt):lineno(lineno),condition(&condition),ifstmt(&ifstmt),elstmt(&elstmt){}
    virtual Value *codegen();
};

class NWhileStmt :public NStmt{
public:
    int lineno;
	virtual void print(int i) const;
    NExp* condition;
    NStmt* body;
    NWhileStmt(int lineno,NExp& condition,NStmt& body):lineno(lineno),condition(&condition),body(&body){}
    virtual Value *codegen();
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
    virtual Value *codegen(){
        return NULL;
    };
};

class NParamDec:public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NSpecifier* spe;
    NVarDec* vardec;
    NParamDec(int lineno,const NSpecifier& spe,NVarDec& vardec):lineno(lineno),spe(&spe),vardec(&vardec){}
    virtual Value *codegen(){
        return NULL;
    };
};

class NFuncDec : public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NIdentifier* id;
    NVarList dlist;
    NFuncDec(int lineno,const NIdentifier& id):lineno(lineno),id(&id) {}
    NFuncDec(int lineno,const NIdentifier& id, NVarList& dlist):lineno(lineno),id(&id),dlist(dlist){}
    virtual Value *codegen(){
        return NULL;
    };
};

class NInteger : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    long long value;
    char* endptr;
    NInteger(int lineno,std:: string& s);
    virtual Value *codegen();
};

class NDouble : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    double value;
    NDouble(int lineno,double value);
    virtual Value *codegen();
};

class NMethodCall : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    const NIdentifier* id;
    NExpList arguments;
    void check();
    NMethodCall(int lineno,const NIdentifier& id, NExpList& arguments) :lineno(lineno),
        id(&id), arguments(arguments) {
        check();
    }
    NMethodCall(int lineno,const NIdentifier& id);
    virtual Value *codegen();
};

class NBinaryOperator : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    int op;
    NExp* lhs;
    NExp* rhs;
    NBinaryOperator(int lineno,NExp& lhs, int op, NExp& rhs);
    virtual Value *codegen();
};

class NUnaryOperator : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    int op;
    NExp* rhs;
    NUnaryOperator(int lineno,NExp& rhs, int op);
    virtual Value *codegen();
};

class NArrayIndex :public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* index;
    const NExp* arr;
    NArrayIndex(int lineno,NExp& arr,NExp& index);
    virtual Value *codegen(){
        return NULL;
    };
};

class NAssignment : public NExp {
public:
    int lineno;
	virtual void print(int i) const;
    NExp* lhs;
    NExp* rhs;
    NAssignment(int lineno,NExp& lhs, NExp& rhs);
    virtual Value *codegen();
};

class NStructMem :public NExp{
public:
    int lineno;
	virtual void print(int i) const;
    const NExp* expr;
    const NIdentifier* member;
    NStructMem(int lineno,const NExp& expr,const std::string member);
    virtual Value *codegen(){
        return NULL;
    };
};
