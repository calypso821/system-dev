#include <stdio.h>
#include <stdlib.h>

// Dinamična alokacija (2D)
void main( int argc, char *argv[])
{
	int i, j;
	
	if ( argc != 3) {
		printf("Uporaba: %s M N\n", argv[0]);
		exit(0);
	}

	int M = atoi(argv[1]);
	int N = atoi(argv[2]);
	
	int **p = NULL;
	
	p = (int **)malloc(M*sizeof(int *));
	if (p == NULL) {
		printf("Alokacija ni uspela.\n");
		exit(0);
	}
	for ( i=0; i<M; i++) {
		p[i] = (int *)malloc(N*sizeof(int)); 
		if ( p[i] == NULL) {
			printf("Alokacija ni uspela.\n");
			exit(0);	
		}
	}

	for ( i=0; i<M; i++)
		for ( j=0; j<N; j++)
			p[i][j] = i*j;
	// ...
	
	for ( i=0; i<M; i++)
		if (p[i])	
			free(p[i]);
			
	if (p)
		free(p);
}

