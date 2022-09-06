#include <stdio.h>

int main(){
    int n = 153;
    int sum = 0;
    int originaln = n;
    while(n>0){
        int lastdigit = n%10;
        sum += pow(lastdigit,3);
        n = n/10;
    }
    if(sum == originaln){
        printf("Armstrong Number");
    }
    
    return 0;
}