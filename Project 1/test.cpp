#include<iostream>
#include<map>
#include<string>
using namespace std;

enum TOKEN{
    ID=1,
    FUCK,
};
int main(){
    map<string,enum TOKEN> imap;

    imap["ID"]=ID;
    imap["FUCK"]=FUCK;

    cout<<imap["ID"]<<endl;
    cout<<imap["FUCKME"]<<endl;
    cout<<imap["FUCK"]<<endl;
    cout<<imap["em"]<<endl;

    double f=123.123;
    cout<<(((unsigned long long)f)!=f)<<endl;
}
