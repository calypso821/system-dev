// Kopiranje iz std. vhoda na std. izhod
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	  { fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 256

int main( int argc, char *argv[])
{
	int fd, n;
	char buf[BUFSIZE];
	
	while( (n = read( STDIN_FILENO, buf, BUFSIZE)) > 0)
		if ( write( STDOUT_FILENO, buf, n) != n)
			err("write")
		
	if ( n < 0)
		err("read")
	
	return 0;
}
