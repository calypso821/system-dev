
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
	
	if ((kljuc = ftok("/home", 'a')) < 0)
		err("ftok")
	printf("kljuc: %d\n", kljuc);
	
	if ((msqid = msgget( kljuc, 0644 | IPC_CREAT)) < 0)
		err("msgget")
		
	printf("msqid: %d\n", msqid);
	
	strcpy( msgbuf.mtext, "Sporocilo 1.");
	msgbuf.mtype = 5;
	
	if ( msgsnd( msqid, &msgbuf, strlen(msgbuf.mtext)+1, 0) < 0) // +1 za null-terminator
		err("msgsnd")
		
	return 0;
}