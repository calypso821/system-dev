#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
	int pid;
	
	if ((pid = fork()) < 0)
		perror("fork");		
	else if ( pid == 0) {	// Otrok
		printf("Otrok: pid: %d, ppid: %d\n", getpid(), getppid());
		sleep(20);
		exit(0);	
	}

	sleep(10);	// Stars
}