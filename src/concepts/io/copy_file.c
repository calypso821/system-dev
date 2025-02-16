// Kopiranje iz std. vhoda na std. izhod
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	  { fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 10000000000

int main( int argc, char *argv[])
{
	int fd1, fd2, n;
	char buf[BUFSIZE];

	if ( argc != 3) {
		printf("Uporaba:  %s  <file1> <file2>\n", argv[0]);
		exit(0);
	}
	
	if ( (fd1 = open( argv[1], O_RDONLY)) < 0)
		err("open1")
	if ( (fd2 = open( argv[2], O_WRONLY | O_CREAT, 0666)) < 0)
		err("open2")
	
	while( (n = read( fd1, buf, BUFSIZE)) > 0)
		if ( write( fd2, buf, n) != n)
			err("write")
		
	if ( n < 0)
		err("read")
	
	if ( close(fd1) < 0)
		err("close1")
	if ( close(fd2) < 0)
		err("close2")
	
	return 0;
}
