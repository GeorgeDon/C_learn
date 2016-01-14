#include"IMalloc.h"
#include<stdio.h>
int main() 
{ 
	int i;
	int*pn=(int*)Imalloc(5*sizeof(int));
	printf("malloc %p\n",pn);
	for(i=0;i<5;i++) pn[i]=i; 
	pn=(int*)Irealloc(pn,10*sizeof(int)); 
	printf("realloc%p\n",pn);
	for(i=5;i<10;i++) pn[i]=i;
	for(i=0;i<10;i++) printf("%3d",pn[i]);
	printf("\n");
	Ifree(pn); 
	return 0; } 


