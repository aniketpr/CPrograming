#include <stdio.h>

int main(){

    char operator;
    printf("Please enter operator only + - * /: ");
    scanf("%c", &operator);
    int num1;
    printf("\nPlease enter num1 : \n");
    scanf("%d", &num1);
    int num2;
    printf("\nPlease enter num2 : ");
    scanf("%d", &num2);
    int result;

    switch (operator)
    {
        case '+':
        {
            result = num1 + num2;
            printf("%d", result);
            break;
        }
        case '-':
        {
            result = num1 - num2;
            printf("%d", result);
            break;
        }
        case '*':
        {
            result = num1 * num2;
            printf("%d", result);
            break;
        }
        case '/':
        {
            result = num1 / num2;
            printf("%d", result);
            break;
        }
        default:
            {
                printf("Invalid Input, Only m or f is allowed");
            }
    }




    return 0;
}