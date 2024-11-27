#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void main()
{
	pid_t pid;
	int statloc;
	
	if ((pid = fork()) < 0)
		perror("fork");		
	else if ( pid == 0) {	// Otrok
		printf("Otrok: pid: %d, ppid: %d\n", getpid(), getppid());
		sleep(10);
		exit(3);	
	}

	wait(&statloc);
	printf("status: %d\n", (statloc >> 8) & 0xff);
	sleep(20);	// Stars
}