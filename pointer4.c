//Pointer To Pointer
#include<stdio.h>
int main(){
    int a = 10;
    int *p;
    p = &a;
    printf("%d\n",*p);
    int **q = &p;
    printf("%p\n", *q);
    printf("%d\n",**q);
    return 0;
}