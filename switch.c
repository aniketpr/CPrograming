#include <stdio.h>
int main(){

    char gender;
    printf("Please enter m or f :");
    scanf("%c", &gender);
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