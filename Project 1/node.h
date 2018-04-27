#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

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
typedef std::vector<NStmt*> StmtList;
typedef std::vector<NExp*> ExpList;
typedef std::vector<NExtDef*> ExtDefList;
typedef std::vector<NDef*> DefList;
typedef std::vector<NDec*> DecList;
typedef std::vector<NVarDec*> VarDecList;
typedef std::vector<NParamDec*> VarList;

class Node {
public:
    virtual ~Node() {}
    virtual void print(){}
};

class NExtDef : public Node{
};

class NExtDefNormal: public NExtDef{
public:
    const NSpecifier &spe;
    VarDecList extlist;
    NExtDefNormal(const NSpecifier &spe,const VarDecList& extlist):spe(spe),extlist(extlist){}
    NExtDefNormal(const NSpecifier &spe):spe(spe){}
};

class NExtDefFunc :public NExtDef{
public:
    const NSpecifier &spe;
    const NFuncDec& funcdef;
    NCompSt& code;
    NExtDefFunc(const NSpecifier &spe,const NFuncDec& funcdef,NCompSt& code):spe(spe),funcdef(funcdef),code(code){} 
};

class NSpecifier: public Node{
public:
    int type;
    int is_struct;
    NStructSpecifier& spe;
    NSpecifier(int type):type(type),is_struct(0){}
    NSpecifier(NStructSpecifier& spe):is_struct(1),spe(spe){}
};

class NStructSpecifier: public Node{
public:
    const NIdentifier& id;
    DefList defList;
    NStructSpecifier(const NIdentifier& id,const DefList& defList):id(id),defList(defList){}
    NStructSpecifier(const NIdentifier& id):id(id){};
};
class NStmt : public Node {
};

class NExp : public NStmt {
};

class NCompSt: public NStmt{
public:
    StmtList klist;
    DefList defList;
    NCompSt(){}
    void add(DefList& dlist,StmtList& slist){
        for(auto iter=dlist.begin();iter!=dlist.end();iter++){
            defList.push_back(*iter);
        }
        for(auto iter=slist.begin();iter!=slist.end();iter++){
            klist.push_back(*iter);
        }
    }
};

class NDef :public NStmt{
public:
    const NSpecifier& spe;
    DecList list;
    NDef(const NSpecifier& spe,const DecList& list):spe(spe),list(list){}
};

class NDec :public NStmt{
public:
    const NVarDec& vardec;
    const NExp&  expr;
    int is_assign;

    NDec(const NVarDec& vardec):vardec(vardec),is_assign(0){}
    NDec(const NVarDec& vardec,const NExp&  expr):expr(expr),is_assign(1),vardec(vardec){}
};

class NReturnStmt: public NStmt{
public:
    NExp& res;
    NReturnStmt(NExp& res):res(res){}
};

class NIfStmt : public NStmt{
public:
    NExp& condition;
    NStmt& ifstmt;
    NStmt& elstmt;
    NIfStmt(NExp& condition,NStmt& ifstmt):condition(condition),ifstmt(ifstmt){}
    NIfStmt(NExp& condition,NStmt& ifstmt,NStmt& elstmt):condition(condition),ifstmt(ifstmt),elstmt(elstmt){}
};

class NWhileStmt :public NStmt{
public:
    NExp& condition;
    NStmt& body;
    NWhileStmt(NExp& condition,NStmt& body):condition(condition),body(body){}
};

class NIdentifier : public NExp{
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

class NVarDec : public NExp{
public:
    const NIdentifier& id;
    NVarDec& next;
    int is_arr;
    int length;
    NVarDec(const NIdentifier& id):id(id),is_arr(0){}
    NVarDec(NVarDec& next,int length):next(next){
        this->next.is_arr=1;
        this->next.length=length;
    }
};

class NParamDec:public NExp{
public:
    const NSpecifier& spe;
    const NVarDec& vardec;
    NParamDec(const NSpecifier& spe,const NVarDec& vardec):spe(spe),vardec(vardec){}
};

class NFuncDec : public NExp{
public:
    const NIdentifier& id;
    VarList list;
    NFuncDec(const NIdentifier& id):id(id){}
    NFuncDec(const NIdentifier& id,VarList& list):id(id),list(list){}
};

class NInteger : public NExp {
public:
    long long value;
    NInteger(long long value) : value(value) { }
};

class NDouble : public NExp {
public:
    double value;
    NDouble(double value) : value(value) { }
};

class NMethodCall : public NExp {
public:
    const NIdentifier& id;
    ExpList arguments;
    NMethodCall(const NIdentifier& id, ExpList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
};

class NBinaryOperator : public NExp {
public:
    int op;
    NExp& lhs;
    NExp& rhs;
    NBinaryOperator(NExp& lhs, int op, NExp& rhs) :
        op(op),lhs(lhs), rhs(rhs){ }
};

class NUnaryOperator : public NExp {
public:
    int op;
    NExp& rhs;
    NUnaryOperator(NExp& rhs, int op) :
        op(op),rhs(rhs){ }
};

class NArrayIndex :public NExp{
public:
    NExp& index;
    NExp& arr;
    NArrayIndex(NExp& arr,NExp& index):
        index(index),arr(arr){}
};

class NAssignment : public NExp {
public:
    NExp& lhs;
    NExp& rhs;
    NAssignment(NExp& lhs, NExp& rhs) : 
        lhs(lhs), rhs(rhs) { }
};

class NStrutMem :public NExp{
public:
    NExp& expr;
    NIdentifier& member;
    NStrutMem(NExp& expr,NIdentifier& member):expr(expr),member(member){}
};
