#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>

int main(){

	char *b = (char *)malloc(2*sizeof(char));

	strcpy(b, "XY");

	char *c = (char *)malloc(sizeof(char));

	*c = *b + 1;
	printf("the value : %s", c);

	return 0;
}
