#include <stdio.h>

int main(){
    int time;
    printf("Enter time");
    scanf("%d", &time);
    if(time <= 12){
        printf("Good Morning");
    }else if(time >= 13 && time <= 18 ){
        printf("Good Evening");
    }else{
        printf("Good Night");
    }
    return 0;
}