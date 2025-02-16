#include <pthread.h>
#include <stdio.h>

void *funkcija_niti( void *arg)
{
	printf("Pozdrav od niti!\n");
	
	return((void *)7);
}

int main()
{
	pthread_t tid;
	void *retval;
	
	if ( pthread_create( &tid, NULL, funkcija_niti, NULL) != 0)
		perror("pthread_create");
	
	if ( pthread_join( tid, &retval) != 0)
		perror("pthread_join");
		
	printf("retval: %ld\n", (long)retval);
	
	return 0;
}