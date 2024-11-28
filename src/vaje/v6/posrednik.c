#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

struct mymesg {
	long mtype;
	char mtext[256];
};

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

    // Message queue
	int msqid;
	key_t msg_key;
	struct mymesg msgbuf;
	
    // Init msg queue
	if ( (msg_key = ftok("/tmp", 'c')) < 0)
		err("ftok")

	if ((msqid = msgget( msg_key, 0644 | IPC_CREAT)) < 0)
		err("msgget")


    // 1. Get shared memory
	if ((shm_key = ftok("/tmp", 'a'))< 0)
		err("ftok")
	if ((shmid = shmget(shm_key, 2048, 0644)) < 0)
		err("semget")
    // Attch (get segment of memory)
	addr = shmat(shmid, 0 /*jedro izbere naslov*/, 0);
	if (addr == (void *)-1)
		err("shmat")	
	
    // 2. Get semaphor
    if ((sem_key = ftok("/tmp", 'b'))< 0)
		err("ftok")
	// Creates semaphor structure (1)
	if ((semid = semget(sem_key, 1, 0644)) < 0)
		err("semget")
		
    // Set opration values
    operation[0].sem_num = 0;
	operation[0].sem_flg = 0; // Wait for resource (blocking)

    static char last_msg[2048];

    while (1)
    {
        // Decrement (read)
        operation[0].sem_op = -1;
        semop(semid, operation, 1);

        printf("Pre message: ");
        puts(last_msg);
        // Read from memory
        printf("New message: ");
        puts(addr);

        if (strcmp(addr, last_msg) != 0)
        {
            // Msg is valid
            strcpy(msgbuf.mtext, addr);
            msgbuf.mtype = 1;
            
            // Send msg to queue
            if (msgsnd(msqid, &msgbuf, strlen(msgbuf.mtext)+1, 0) < 0) // +1 za null-terminator
                err("msgsnd");
            printf("Message send to queue\n\n");
            strcpy(last_msg, addr);
        }
        else {
            printf("Message is NOT valid\n\n");
        }
        
        // Increment (unlock)
        operation[0].sem_op = 1;
        semop(semid, operation, 1);

        if (addr[0] == '\0')
        {
            printf("Message 0 read, program end!\n");
            break;
        }      
        
        sleep(1);
    }

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


	// // Clean msg queue strucutre
	// if (msgctl( msqid, IPC_RMID, NULL) < 0)
	// 	err("msgctl")

	return 0;
}