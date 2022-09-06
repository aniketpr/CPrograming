#include <stdio.h>
#include <conio.h>
void main()
{
	printf("Enter the password : ");
	int i = 0;
	char ch,pass[30];
	while(1){
		ch=getch();
		if(ch=13){
			pass[i]='\0';
			break;
		}
		pass[i++]=ch;
		printf("*");
	}

	printf("%s",pass);
	getch();
}