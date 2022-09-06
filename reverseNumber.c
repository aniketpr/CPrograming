#include <stdio.h>

int main(){
    int n = 1234;
    int reverse = 0;
    while(n>0){
        int lastdigit = n%10;
        reverse = reverse*10 + lastdigit;
        n = n/10;
    }
    printf("%d",reverse);

    
    return 0;
}