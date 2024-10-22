#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }


int main( int argc, char *argv[])
{
	int fd, n;
	char buf[100];
	
	if ( argc != 2) {
		printf("Uporaba:  %s  <filename> \n", argv[0]);
		exit(0);
	}
	
	if ( (fd = open( argv[1], O_RDONLY)) < 0 )
		err("open")
	
	while ( (n = read( fd, buf, 20)) > 0 ) {
		printf("n: %d\n", n);
	}

	if ( n < 0)
		err("read")

	if ( close(fd)	< 0)
		err("close")
				
	return 0;
}
