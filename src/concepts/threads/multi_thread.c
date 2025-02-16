#include <pthread.h>
#include <stdio.h>

#define N_THR 10

void *funkcija_niti( void *arg)
{
	long st = (long)arg;
	printf("Pozdrav od niti %ld!\n", st);
	
	return(NULL);
}

int main()
{
	pthread_t tid[N_THR];
	void *retval;
	
	for ( long i=0; i<N_THR; i++)	
		if ( pthread_create( &tid[i], NULL, funkcija_niti, (void *)i) != 0)
			perror("pthread_create");
	
	for ( int i=0; i<N_THR; i++)
		if ( pthread_join( tid[i], &retval) != 0)
			perror("pthread_join");
	
	return 0;
}