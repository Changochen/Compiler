#include <iostream>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>

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
class NVarDecList;
class NVarList;

class Node {
public:
    virtual void print(int i){}
};

static Node tmp;

class NExtDef : public Node{

};

class NExtDefList:public Node{
public:
    std::vector<NExtDef*> vec;
    NExtDefList(){}
    void push_back(NExtDef* ptr){
        vec.push_back(ptr);
    }
};

class NStmtList:public Node{
public:
    std::vector<NStmt*> vec;
    NStmtList(){}
    void push_back(NStmt* ptr){
        vec.push_back(ptr);
    }
};

class NVarList:public Node{
public:
    std::vector<NParamDec*> vec;
    NVarList(){}
    void push_back(NParamDec* ptr){
        vec.push_back(ptr);
    }
};

class NVarDecList:public Node{
public:
    std::vector<NVarDec*> vec;
    NVarDecList(){}
    void push_back(NVarDec* ptr){
        vec.push_back(ptr);
    }
};

class NDecList:public Node{
public:
    std::vector<NDec*> vec;
    NDecList(){}
    void push_back(NDec* ptr){
        vec.push_back(ptr);
    }
};

class NDefList:public Node{
public:
    std::vector<NDef*> vec;
    NDefList(){}
    void push_back(NDef* ptr){
        vec.push_back(ptr);
    }
};

class NExpList:public Node{
public:
    std::vector<NExp*> vec;
    NExpList(){}
    void push_back(NExp* ptr){
        vec.push_back(ptr);
    }
};

class NBlock: public Node{
public:
    NExtDefList* llist;
    NBlock(NExtDefList &llist):llist(&llist){}
};

class NExtDefNormal: public NExtDef{
public:
    const NSpecifier* spe;
    NVarDecList extlist;
    NExtDefNormal(const NSpecifier &spe,const NVarDecList& extlist):spe(&spe),extlist(extlist){}
    NExtDefNormal(const NSpecifier &spe):spe(&spe){}
};

class NExtDefFunc :public NExtDef{
public:
    const NSpecifier* spe;
    const NFuncDec* funcdef;
    const NCompSt* code;
    NExtDefFunc(const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code):spe(&spe),funcdef(&funcdef),code(&code){} 
};

class NStructSpecifier: public Node{
public:
    const NIdentifier* id;
    NDefList defList;
    NStructSpecifier(const NIdentifier& id,const NDefList& defList):id(&id),defList(defList){}
    NStructSpecifier(const NIdentifier& id):id(&id){};
};


class NSpecifier: public Node{
public:
    int type;
    int is_struct;
    NStructSpecifier* spe;

    NSpecifier(int type):type(type),is_struct(0){}
    NSpecifier(NStructSpecifier& spe):is_struct(1),spe(&spe){}
};


class NStmt : public Node {
};

class NExp : public NStmt {
};

class NCompSt: public NStmt{
public:
    NStmtList klist;
    NDefList defList;
    NCompSt(){}
    void add(NDefList& dlist,NStmtList& slist){
        for(auto iter=dlist.vec.begin();iter!=dlist.vec.end();iter++){
            defList.push_back(*iter);
        }
        for(auto iter=slist.vec.begin();iter!=slist.vec.end();iter++){
            klist.push_back(*iter);
        }
    }
};

class NDef :public NStmt{
public:
    const NSpecifier* spe;
    NDecList list;
    NDef(const NSpecifier& spe,const NDecList& list):spe(&spe),list(list){}
};

class NDec :public NStmt{
public:
    const NVarDec* vardec;
    const NExp*  expr;
    int is_assign;

    NDec(const NVarDec& vardec):vardec(&vardec),is_assign(0){}
    NDec(const NVarDec& vardec,const NExp&  expr):expr(&expr),is_assign(1),vardec(&vardec){}
};

class NReturnStmt: public NStmt{
public:
    NExp& res;
    NReturnStmt(NExp& res):res(res){}
};

class NIfStmt : public NStmt{
public:
    const NExp* condition;
    const NStmt* ifstmt;
    const NStmt* elstmt;
    NIfStmt(NExp& condition,NStmt& ifstmt):condition(&condition),ifstmt(&ifstmt){}
    NIfStmt(NExp& condition,NStmt& ifstmt,NStmt& elstmt):condition(&condition),ifstmt(&ifstmt),elstmt(&elstmt){}
};

class NWhileStmt :public NStmt{
public:
    const NExp* condition;
    const NStmt* body;
    NWhileStmt(NExp& condition,NStmt& body):condition(&condition),body(&body){}
};

class NIdentifier : public NExp{
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

class NVarDec : public NExp{
public:
    const NIdentifier* id;
    NVarDec* next;
    int is_arr;
    int length;
    NVarDec(const std::string& id):is_arr(0){
        this->id=new NIdentifier(id);
    }
    NVarDec(NVarDec& next,int length):next(&next){
        this->next->is_arr=1;
        this->next->length=length;
    }
};

class NParamDec:public NExp{
public:
    const NSpecifier* spe;
    const NVarDec* vardec;
    NParamDec(const NSpecifier& spe,const NVarDec& vardec):spe(&spe),vardec(&vardec){}
};

class NFuncDec : public NExp{
public:
    const NIdentifier* id;
    NVarList list;
    NFuncDec(const NIdentifier& id):id(&id){}
    NFuncDec(const NIdentifier& id,NVarList& list):id(&id),list(list){}
};

class NInteger : public NExp {
public:
    long long value;
    char* endptr;
    NInteger(std:: string& s){
        std::printf("Parsing %s\n",s.c_str());
        endptr=NULL;
        value=std::strtol(s.c_str(),&endptr,0);
        if(endptr!=NULL){
            std::printf("Error type A at Line %d: \'Unknown Integer %s\'\n", yylineno, s.c_str());
            std::exit(1);
        }
        std::printf("Get value %lld\n",value);
    }
};

class NDouble : public NExp {
public:
    double value;
    NDouble(double value) : value(value) { }
};

class NMethodCall : public NExp {
public:
    const NIdentifier* id;
    NExpList arguments;
    NMethodCall(const NIdentifier& id, NExpList& arguments) :
        id(&id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(&id) { }
};

class NBinaryOperator : public NExp {
public:
    int op;
    const NExp* lhs;
    const NExp* rhs;
    NBinaryOperator(NExp& lhs, int op, NExp& rhs) :
        op(op),lhs(&lhs), rhs(&rhs){ }
};

class NUnaryOperator : public NExp {
public:
    int op;
    const NExp* rhs;
    NUnaryOperator(NExp& rhs, int op) :
        op(op),rhs(&rhs){ }
};

class NArrayIndex :public NExp{
public:
    const NExp* index;
    const NExp* arr;
    NArrayIndex(NExp& arr,NExp& index):
        index(&index),arr(&arr){}
};

class NAssignment : public NExp {
public:
    const NExp* lhs;
    const NExp* rhs;
    NAssignment(NExp& lhs, NExp& rhs) : 
        lhs(&lhs), rhs(&rhs) { }
};

class NStrutMem :public NExp{
public:
    const NExp* expr;
    const NIdentifier* member;
    NStrutMem(NExp& expr,NIdentifier& member):expr(&expr),member(&member){}
};
