#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/msg.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main()
{
	union semun arg;
	key_t key;
	int semid;
		
	if ((key = ftok("/usr", 'b'))< 0)
		err("ftok")
	// Semaphor (1)
	if ( (semid = semget( key, 1, 0644 | IPC_CREAT)) < 0)
		err("semget")
	int msqid; 
	// Message queue
	if ( (msqid = msgget( key, 0644 | IPC_CREAT)) < 0)
		err("semget")
	printf("%d, %d\n", semid, msqid);		
		
	// Init semaphr (index 0)
	arg.val = 0;
	if ( (semctl( semid, 0, SETVAL, arg)) < 0)
		err("semctl")
	
	return 0;
}