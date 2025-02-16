#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }


void main( int argc, char *argv[])
{
	struct stat buf;

	if ( argc != 2) 
		err("argc != 2")
			
	if ( stat( argv[1], &buf) < 0)
		err("stat")

	if ( S_ISREG(buf.st_mode) )
		printf("Regular file\n");
	else if ( S_ISDIR(buf.st_mode) )
		printf("Direktorij\n");
	else if ( S_ISCHR(buf.st_mode) )
		printf("Character driver\n");
	else
		printf("Nekaj drugega.\n");
	
}
