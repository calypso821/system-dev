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
		write( pfd[1], "Pozdrav1\n", sizeof("Pozdrav1\n"));	
		write( pfd[1], "Pozdrav2\n", sizeof("Pozdrav2\n"));	
		wait(NULL);
	}
	else {	// OTROK (pid=0)
		n = read( pfd[0], buf, BUFSIZE);
		write( STDOUT_FILENO, buf, n);
	}		

	return 0;	
}