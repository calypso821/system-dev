// fork3.c:
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
	fork();
	printf("Fork izveden\n");
	sleep(5);

	fork();
	printf("Fork izveden\n");
	sleep(5);

	fork();
	printf("Fork izveden\n");
	sleep(5);
}