
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

int globalna = 0;
int K = 1000;

void *funkcija_niti(void *arg)
{
	int i,j;
	
	for ( i=0; i<K; i++ ) { // karikiran primer ne-atomskega dostopa
		j = globalna;
		j--;
		printf("b");
		fflush(stdout);
		//sleep(1);
		globalna = j;
	}
	return NULL;
}

int main(void)
{
	pthread_t id;
	int i;
	if ( pthread_create(&id, NULL, funkcija_niti, NULL) )
		err("pthread_create")
	
	for ( i=0; i<K; i++) {
		globalna++;
		printf("a");
		fflush(stdout);
		//sleep(1);
	}

	if ( pthread_join(id, NULL ) )
		err("pthread_join")

	printf("\nglobalna: %d\n", globalna); // dobimo 11
	exit(0);
}