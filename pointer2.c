#include<stdio.h>
// Pointer Arithmetic Supports ++, --, +,- 
int main(){
    int a =10;
    int *aptr = &a;
    printf("%p\n", aptr); // it will print address for ex:-2000
    aptr++; 
    //changing the place of variable i.e updating the address.
    printf("%p\n", aptr);// it will print address for ex:-2004 bcz int takes 4 bytes of place.

    char ch = 'a';
    char *cptr = &ch;
    printf("%p\n", cptr); // it will print address for ex:-3000
    cptr++; 
    //changing the place of variable i.e updating the address.
    printf("%p\n", cptr);// it will print address for ex:-3001 bcz char takes 1 bytes of place.

    return 0;
}