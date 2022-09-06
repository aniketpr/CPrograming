#include <stdio.h>

int square(int side){
    int area_of_square = side * side;
    printf("Area of Square = %d\n", area_of_square);
    return 0;
}
int rectangle(int length, int breadth){
    square(8);
    int areaOfRectangle = length * breadth;
    printf("Area of Rectangle = %d", areaOfRectangle);
    return 0;
}

void voteChecker(int age){
    if(age >= 18){
        printf("You Can Vote!!!");
    }else{
        printf("You Can Not Vote!!!");
    }
}

void evenOdd(int number){
    if(number % 2 == 0){
        printf("It is even\n");
    }else{
        printf("It is odd\n");
    }
}

int genderChecker(char gender){
    switch(gender){
        case 'm':
        {
            printf("Male");
            break;
        }
        case 'M':
        {
            printf("Male");
            break;
        }
        case 'f':
        {
            printf("Female");
            break;
        }
        case 'F':
        {
            printf("Female");
            break;
        }
        default:
        {
            printf("Invalid Input, Only m or f is allowed");
        }
    }
    return 0;
}

int main(){
    char gender;
    printf("Please enter m or f :");
    scanf("%c", &gender);
    genderChecker(gender);
    return 0;
}

