#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int system(const char *cmdstring) /* verzija brez signalov */
{
	pid_t pid;
	int status;
	
	if (cmdstring == NULL)
		return(1);
	
	if ( (pid = fork()) < 0)
		status = -1; /* zmanjkalo procesov */
	else if (pid == 0) { /* otrok */
		execl("/bin/sh", "sh", "-c", cmdstring, (char *) 0);
		_exit(127); /* napaka pri execl */
	}
	
	/* starš */
	waitpid(pid, &status, 0);
	
	return(status);
} 

int main()
{
	system("date > file");
}
