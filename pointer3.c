// Pointer and Array
//Note: we can not increase or decrease a pointer in array elements
#include<stdio.h>
int main(){
    int arr[] = {10,20,30};
    printf("%d\n",*arr);
    int *ptr=arr;
    for(int i=0;i<3;i++){
        printf("%d\n",*ptr);
        ptr++;
    }
    // for(int i=0;i<3;i++){
    //     printf("%d\n",*(arr+i)); // we are using +i bcz arr points to index not the memory address
    //     //arr++ //illegal
    // }
    
    return 0;
}