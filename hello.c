#include <stdio.h>
#include "inc/lm3s9b92.h"
/*
 * hello.c
 */
int fib(int n)
{
	if (n==1 || n==0)
		return 1;
	else return fib(n-1)+fib(n-2);
}

int main(void)
{
	printf("Hello World!\n");
	printf("HelloMuchacho!\n");
	int i = 1;
	for (i = 0; i < 5; i++)
	{
		printf("%d\n",fib(i));
	}
}
