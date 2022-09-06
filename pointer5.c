// Passing pointers to function
#include<stdio.h>
void increment(int a){
    a++;
}
int main(){
    int a=2;
    increment(a);
    printf("%d\n", a);
    return 0;
}