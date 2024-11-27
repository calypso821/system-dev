
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main()
{
    // Shared memory
	key_t shm_key;
	int shmid;
	char *addr;	

    // Semaphor
    union semun arg;
	key_t sem_key;
	int semid;
    struct sembuf operation[1];
	
    // 1. Init shared memory
	if ((shm_key = ftok("/tmp", 'c'))< 0)
		err("ftok")

    // Create shared memory strucutre
	if ((shmid = shmget(shm_key, 2048, 0644 | IPC_CREAT)) < 0)
		err("semget")
    // Attch (get segment of memory)
	addr = shmat( shmid, 0 /*jedro izbere naslov*/, 0);
	if ( addr == (void *)-1 )
		err("shmat")	
	
    // 2. Init Semaphor
    if ((sem_key = ftok("/usr", 'b'))< 0)
		err("ftok")
	// Creates semaphor structure (1)
	if ((semid = semget(sem_key, 1, 0644 | IPC_CREAT)) < 0)
		err("semget")

    // Init semaphor (index 0)
	arg.val = 1; // Initial value
	if ((semctl(semid, 0, SETVAL, arg)) < 0)
		err("semctl")

    // Set opration values
    operation[0].sem_num = 0;
	operation[0].sem_flg = IPC_NOWAIT;

    // Write to shared memory 
    for (int i = 0; i < 4; i++)
    {
        // Dectrement (lock)
        operation[0].sem_op = -1;
        semop(semid, operation, 1);

        sprintf(addr, "ID %d: To je sporocilo!\n", i);
        sleep(2);

        // Increment (unlock)
        operation[0].sem_op = 1;
        semop(semid, operation, 1);

        sleep(2);
    }
    addr[0] = '\0';  // Clear the message
    
    // Clean 
    // Detach memory
	if (shmdt(addr) < 0)
		err("shmdt")
    // Remove shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        err("shmctl");
    }
    // Clean semaphor strucutre
    if (semctl(semid, 0, IPC_RMID) < 0) {
        err("semctl");
    }
	
	return 0;
}