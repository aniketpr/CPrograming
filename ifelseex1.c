// agar input 'm' tho Male print karna chaiye
// agar input 'f' tho Female print karna chaiye
// aur sirf 'if' statement ka hi use karna hai

#include <stdio.h>
int main(){
    char gender;
    printf("Please enter m or f :");
    scanf("%c", &gender);
    if(gender == 'm' || gender == 'M'){
        printf("Male");
    }
    if(gender == 'f' || gender == 'F'){
        printf("Female");
    }
    return 0;
}

// T or T => T 
// T or F => T 
// F or T => T 
// F or F => F 
// T and T => T
// T and F => F
// F and T => F
// F and F => F