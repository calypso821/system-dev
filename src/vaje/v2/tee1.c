#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	  { fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 256

int main(int argc, char* argv[]) {
    if (argc !=2 && argc !=3) {
        printf("Uporaba: %s <file>\n%s -a <file>\n", argv[0], argv[0]);
        return 1;
    }

    char buf[BUFSIZE];
    int fd, n;

     
    if (argc == 2) {
        fd = open(argv[1], O_TRUNC | O_WRONLY);
    } else if (argc == 3) {
        fd = open(argv[2], O_APPEND | O_WRONLY);
    }

	while((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
		if ( write(fd, buf, n) != n)
			err("write");

    if (close(fd) < 0)
        err("close1");

    // char fname[256];
    // // Kopiranje po blokih (stdvhod)
    // // Dokler ne najdemo EOF
    
    // if (argc == 2) {
    //     //file = open(arv[1], O);
    //     strcpy(fname, argv[1]);
    // } else if (argc == 3) {
    //     // Append
    //     strcpy(fname, argv[2]);
    // }
}