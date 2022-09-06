//Array = It is a used  to store homogenous/same type of data collection;

#include <stdio.h>

int main(){
    int a[] = {2,3,6,7}; // array declaration and every element has it index which starts from 0,...
    // printf("%d\n", a[0]); // a[index] array calling the particular element
    // printf("%d\n", a[1]);
    // printf("%d\n", a[2]);
    // printf("%d\n", a[3]);
    for(int i = 0; i <= 3; i++){ 
        printf("%d\n", a[i]); 
    }
    return 0;
}