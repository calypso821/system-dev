#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 256

int main()
{
	int pfd[2], n;
	char buf[BUFSIZE];

	if ( pipe(pfd) < 0)
		err("pipe")
	
	write( pfd[1], "Pozdrav!", strlen("Pozdrav!"));
	n = read( pfd[0], buf, BUFSIZE);
	buf[n] = 0;
	printf("strlen: %ld\n", strlen("Pozdrav!"));
	
	puts(buf);
	
}