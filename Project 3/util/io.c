#include<stdio.h>

int output;

int readint32(){
    int res;
    scanf("%d",&res);
    return res;
}

int writeint32(){
    printf("%d\n",output);
    return 0;
}
