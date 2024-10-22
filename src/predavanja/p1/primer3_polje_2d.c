#include <stdio.h>

void main()
{
	int a[2][3] = {{10, 20, 30}, {50, 60, 70}};
	//int *ptr = a;

	printf("a: %p\n", a); 
	printf("a[0]: %p, a[1]: %p\n", a[0], a[1]);
	printf("a[0][0]: %d, a[0][1]: %d\n", a[0][0], a[0][1]);
}
