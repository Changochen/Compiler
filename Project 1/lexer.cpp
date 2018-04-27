#include<iostream>
#include<string>
#include<map>
#include<cstdio>
#include"def.h"
#include<fstream>
using namespace std;

#define END 0xdeadbeef

static string id_string;
static unsigned long lineno;
static double NumVal;
static map<string,enum TOKEN> imap;
static ifstream input;
static void initialize(string file){
   imap[";"]=SEMI;
   imap[","]=COMMA;
   imap["="]=ASSINGOP;
   imap[">"]=RELOP;
   imap["<"]=RELOP;
   imap["<="]=RELOP;
   imap[">="]=RELOP;
   imap["!="]=RELOP;
   imap["=="]=RELOP; imap["+"]=PLUS;
   imap["-"]=MINUS;
   imap["*"]=STAR;
   imap["/"]=DIV;
   imap["&&"]=AND;
   imap["||"]=OR;
   imap["."]=DOT;
   imap["!"]=NOT;
   imap["int"]=TYPE;
   imap["float"]=TYPE;
   imap["("]=LP;
   imap[")"]=RP;
   imap["["]=LB;
   imap["]"]=RB;
   imap["{"]=LC;
   imap["}"]=RC;
   imap["struct"]=STRUCT;
   imap["return"]=RETURN;
   imap["if"]=IF;
   imap["else"]=ELSE;
   imap["while"]=WHILE;
   input.open(file);
   if(input.is_open()==false){
       cout<<"Can not open file "<<file <<endl;
       exit(0);
   }
}

static int gettok(){
    static char LastChar=' ';
    while(isspace(LastChar)||LastChar=='\n'){
        if(LastChar=='\n')lineno++;
        if(!input.get(LastChar)){
            cout<<"End of file"<<endl;
            return END;
        };
    }
    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        id_string = LastChar;
        while (input.get(LastChar)&&isalnum(LastChar))
            id_string += LastChar;

        if(id_string=="int"||id_string=="float"||id_string=="struct"||id_string=="if"||id_string=="return"||id_string=="else"||id_string=="while"){
            return imap[id_string];
        }else{
            return ID;
        }
    }
    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;

        } while(input.get(LastChar)&&(isdigit(LastChar) || LastChar == '.'));

        /* To be fix */
        NumVal = strtod(NumStr.c_str(), nullptr);
        if(((unsigned long long)NumVal)!=NumVal){
            return FLOAT;
        }
        else{
            return INT;
        }
    }
    
    if(LastChar=='<'||LastChar=='>'||LastChar=='='||LastChar=='!'){
        id_string = LastChar;
        if(input.get(LastChar)&&LastChar=='='){
            id_string+=LastChar;
        }else{
            input.unget();
        }
        return imap[id_string];
    }

    if(LastChar=='|'||LastChar=='&'){
        if(input.get(LastChar)&&LastChar=='='){
            id_string+=LastChar;
            return imap[id_string];
        }else{
            input.unget();
        }    
    }
    if(imap[id_string]){
        return imap[id_string];
    }
    cout<<"Unknown token "<<id_string<<"at line " <<lineno <<endl;
    return -1;
}


