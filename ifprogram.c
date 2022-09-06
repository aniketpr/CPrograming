#include <stdio.h>

// ==
// >=
// <=
// <
// >

//Main is function 
int main() {

    int age;
    printf("Enter Your age : \n");
    scanf("%d", &age);
    if (age <= 17){
        printf("You are not eligible to vote");
    }else{
        printf("You are eligible to vote");
    }

    return 0; // it will exit the program
    
}

