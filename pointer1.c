#include<stdio.h>
int main(){
    int a =10;
    int *aptr = &a;
    printf("%d\n", *aptr);
    *aptr = 20; // updating the value of variable a with the help of pointer
    // Note: We can access the value and modify the value of varible using pointer
    printf("%d\n", a);
    return 0;
}