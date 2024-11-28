
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	if ((shm_key = ftok("/tmp", 'a'))< 0)
		err("ftok")
    printf("key: %d\n", shm_key);
    // Create shared memory strucutre
	if ((shmid = shmget(shm_key, 2048, 0644 | IPC_CREAT)) < 0)
		err("semget")
    // Attch (get segment of memory)
	addr = shmat(shmid, 0 /*jedro izbere naslov*/, 0);
	if (addr == (void *)-1)
		err("shmat")	
	
    // 2. Init Semaphor
    if ((sem_key = ftok("/tmp", 'b'))< 0)
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
	operation[0].sem_flg = 0; // Wait for resource (blocking)

    // Write to shared memory 
    for (int i = 0; i < 5; i++)
    {
        // Dectrement (lock)
        operation[0].sem_op = -1;
        semop(semid, operation, 1);
        printf("Memory: locked\n");

        sprintf(addr, "(ID %d) To je sporocilo!", i);
        printf("Message: ");
        puts(addr);
        printf("Message written to memory\n");
        sleep(2);

        // Increment (unlock)
        operation[0].sem_op = 1;
        semop(semid, operation, 1);
        printf("Memory: unlocked\n\n");

        sleep(2);
    }

    // Dectrement (lock)
    operation[0].sem_op = -1;
    semop(semid, operation, 1);
    printf("Memory: locked\n");

    addr[0] = '\0';  // Clear the message
    printf("Message 0 written to memory\n");
    sleep(2);

    // Increment (unlock)
    operation[0].sem_op = 1;
    semop(semid, operation, 1);
    printf("Memory: unlocked\n\n");
    
	
	return 0;
}