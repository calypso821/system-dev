#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void main()
{
	pid_t pid;
	int status;
	
	pid = fork();
	if ( pid < 0)
		perror("fork");		
	else if ( pid == 0) {	// Otrok
		execlp("ls", "ls", "-l", (char *)0); 
	}

	// Stars
	wait(&status);
}
