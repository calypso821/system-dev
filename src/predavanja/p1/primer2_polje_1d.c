#include <stdio.h>

void main()
{
	int a[3] = {10, 20, 30};
	int *ptr = a;

	for ( int i=0; i<3; i++) {
		printf("ptr: %p, *ptr: %d, a[%d]: %d\n", ptr, *ptr, i, a[i]);
		ptr++;
	}
}
