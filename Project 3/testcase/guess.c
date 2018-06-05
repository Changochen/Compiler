int guess;
int res;
int main(){
    res=0;
    guess=0x233;
    while(guess!=res){
        res=readint32();
    }
    output=guess;
    writeint32();
    return 0;
}
