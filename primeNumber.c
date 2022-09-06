#include <stdio.h>

int main(){
    int n = 131;
    int flag = 0;
    for(int i=2; i<=sqrt(n);i++){
        if(n%i==0){
            printf("Non Prime");
            flag = 1;
            break;
        }
    }
    if(flag==0){
        printf("Prime");
    }

    return 0;
}