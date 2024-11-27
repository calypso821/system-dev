
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main( int argc, char *argv[])
{
	union semun arg;
	key_t key;
	int semid;
	struct sembuf operacije[1];
		
	if ( argc != 2) {
		printf("Uporaba: %s <n>\n"
				 "n je inkrement ali dekrement\n", argv[0]);
		exit(1);
	}
		
	if ((key = ftok("/usr", 'b'))< 0)
		err("ftok")
		
	if ( (semid = semget( key, 1, 0644 | IPC_CREAT)) < 0)
		err("semget")
		
	operacije[0].sem_num = 0;
	operacije[0].sem_op = atoi(argv[1]);
	//operacije[0].sem_flg = 0;
	operacije[0].sem_flg = IPC_NOWAIT;

	if ( semop( semid, operacije, 1) < 0)
		err("semop")
	
	int vrednost_sem;
	if ( (vrednost_sem = semctl( semid, 0, GETVAL, arg)) < 0)
		err("semctl")
	printf("vrednost_sem: %d\n", vrednost_sem);

	return 0;
}