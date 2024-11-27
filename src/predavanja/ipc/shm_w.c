
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

int main()
{
	key_t key;
	int shmid;
	char *addr;	
		
	if ((key = ftok("/tmp", 'c'))< 0)
		err("ftok")

	if ( (shmid = shmget( key, 2048, 0644 | IPC_CREAT)) < 0)
		err("semget")

	addr = shmat( shmid, 0 /*jedro izbere naslov*/, 0);
	if ( addr == (void *)-1 )
		err("shmat")	
	
	strcpy( addr, "To so podatki, To so podatki, To so podatki, To so podatki\n");
	
	if ( shmdt(addr) < 0)
		err("shmdt")
	
	return 0;
}