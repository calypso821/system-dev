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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        return 0;
    }

    // Message queue
	int msqid;
	key_t msg_key;
	struct mymesg msgbuf;
	
    // Init msg queue
	if ( (msg_key = ftok("/tmp", 'c')) < 0)
		err("ftok")

	if ((msqid = msgget(msg_key, 0644)) < 0)
		err("msgget")

    // Open file
    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) {
        err("failed opening file");
    }

    while (1)
    {
        if (msgrcv(msqid, &msgbuf, sizeof(msgbuf.mtext), 0, 0) < 0)
		    err("msgrcv")

        // Check for empty message first
        if (msgbuf.mtext[0] == '\0') {
            printf("Empty message received, program end!\n");
            break;
        }

        printf("Message received: ");
	    puts(msgbuf.mtext);

        // Write message to file
        if (fputs(msgbuf.mtext, fp) == EOF) {
            err("fputs failed");
        }
        // Add new line
        if (fputc('\n', fp) == EOF) {
            err("fputc failed");
        }
    }

    // Close file
    if (fclose(fp) != 0) {
        err("failed closing file");
    }

	// Clean msg queue strucutre
	if (msgctl( msqid, IPC_RMID, NULL) < 0)
		err("msgctl")

	return 0;
}