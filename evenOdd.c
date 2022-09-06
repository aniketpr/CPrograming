#include <stdio.h>

int main(){
    int number;
    printf("Enter any number : ");
    scanf("%d", &number);
    if(number % 2 == 0){
        printf("It is even \n");
    }else{
        printf("It is odd");
    }

    return 0;
}
