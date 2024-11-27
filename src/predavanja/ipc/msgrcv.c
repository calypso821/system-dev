
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

struct mymesg {
	long mtype;
	char mtext[256];
};

int main()
{
	int msqid;
	key_t kljuc;
	struct mymesg msgbuf;
	
	if ( (kljuc = ftok("/home", 'a')) < 0)
		err("ftok")
	printf("kljuc: %d\n", kljuc);

	if ((msqid = msgget( kljuc, 0644)) < 0)
		err("msgget")
		
	printf("msqid: %d\n", msqid);
		
	//if ( msgrcv( msqid, &msgbuf, sizeof(msgbuf.mtext), 5, 0) < 0)
	//if ( msgrcv( msqid, &msgbuf, sizeof(msgbuf.mtext), 3, IPC_NOWAIT) < 0)
	if ( msgrcv( msqid, &msgbuf, sizeof(msgbuf.mtext), 0, IPC_NOWAIT) < 0)
		err("msgrcv")

	puts(msgbuf.mtext);

	// Odstranitev vrste
	if ( msgctl( msqid, IPC_RMID, NULL) < 0)
		err("msgctl")

	return 0;
}