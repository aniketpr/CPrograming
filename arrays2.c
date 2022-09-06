#include <stdio.h>

int main(){

    int a[5]; // declaration of Empty Array
    // a[0] = 5;
    // a[1] = 8;
    // a[2] = 9;
    // a[3] = 10;
    // a[4] = 99;
    printf("Enter 5 element \n");
    for(int i = 0; i <= 4; i++){
        scanf("%d", &a[i]);
    }
    printf("Printing Array Elements Now........\n");
    for(int i = 0; i <= 4; i++){ 
        printf("%d\n", a[i]); 
    }

    return 0;
}