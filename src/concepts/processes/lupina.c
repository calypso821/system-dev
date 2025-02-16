#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define err_sys(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

#define BUFSIZE 512

int main()
{
	pid_t pid;
	char buf[BUFSIZE];
	
	printf("$ ");
	while ( fgets( buf, BUFSIZE, stdin) != NULL) {
		buf[strlen(buf)-1] = 0;
		
		if ( (pid = fork()) < 0)
			err_sys("fork")
		else if ( pid == 0) { // Otrok
			execlp(buf, buf, (char *)0);
			printf("Ne morem izvesti ukaza ...\n");
			exit(127);
		}

		if ( waitpid(pid, NULL, 0) < 0)	// Stars
			perror("waitpid");
		printf("$ ");
	}

	return 0;
}