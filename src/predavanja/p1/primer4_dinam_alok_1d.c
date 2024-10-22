#include <stdio.h>
#include <stdlib.h>

// Dinamična alokacija (1D)
void main( int argc, char *argv[])
{
	if ( argc != 2) {
		printf("Uporaba: %s N\n", argv[0]);
		exit(0);
	}

	int N = atoi(argv[1]);
	
	int *p = NULL;
	
	p = (int *)malloc(N*sizeof(int));
	if (p == NULL) {
		printf("Alokacija ni uspela.\n");
		exit(0);
	}

	for ( int i=0; i<N; i++)
		p[i] = i;
	
	if (p)
		free(p);
}

