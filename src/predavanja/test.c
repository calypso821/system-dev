#include <stdio.h>

void main()
{
	int x = 10;
	int *ptr;

	ptr = &x;	
	printf("x: %d, &x: %p, ptr: %p, *ptr: %d\n", x, &x, ptr, *ptr);
	
	*ptr = 20;
	printf("x: %d, &x: %p, ptr: %p, *ptr: %d\n", x, &x, ptr, *ptr);

}