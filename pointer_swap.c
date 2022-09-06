#include<stdio.h>
void swap(int *a, int *b){
    int temp=*a;
    *a=*b;
    *b=temp;
}
int main(){
    int a=2;
    int b=4;
    printf("Before Swap a=%d and b=%d\n",a,b);
    swap(&a,&b);// call by reference
    // int *aptr=&a;
    // int *bptr=&b;
    // swap(aptr,bptr);// call by reference => means by sending pointer or address insteaded of value
    printf("After Swap a=%d and b=%d\n",a,b);
    return 0;
}