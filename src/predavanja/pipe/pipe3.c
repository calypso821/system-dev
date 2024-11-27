#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 256

int main()
{
	int pfd[2], n;
	char buf[BUFSIZE];

	if ( pipe(pfd) < 0)
		err("pipe")
	
	pid_t pid = fork();
	if ( pid < 0) 
		err("fork")
	else if ( pid > 0) { // STARS
		close(pfd[0]);
		while ( fgets( buf, BUFSIZE, stdin) != NULL) {	// Bere niz iz stdin
			n = strlen(buf);
			write( pfd[1], buf, n);	// Pošlje niz v cev
		}
		close(pfd[1]);
		wait(NULL);
	}
	else {	// OTROK (pid=0)
		close(pfd[1]);
		dup2(pfd[0], STDIN_FILENO);
		close(pfd[0]);
		execlp("grep", "grep", "buf", (char *)0);
	}		

	return 0;	
}