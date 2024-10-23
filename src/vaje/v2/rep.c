#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	  { fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define BUFSIZE 256


int main(int argc, char* argv[]) {
    if (argc !=2 && argc !=4) {
        printf("Uporaba: %s <file>\n%s -n value <file>\n", argv[0], argv[0]);
        return 1;
    }

    char buf[BUFSIZE];
    int fd, n;

    char *path;

    if (argc == 2) {
        path = argv[1]; 
        n = 5;
    } else if (argc == 4 && argv[1] == '-n') {
        path = argv[3];
        n = atoi(argv[2]);
    }

    fd = open(path, O_RDONLY);

    int pos = lseek(fd, 0, SEEK_END);
    int cnt_new_line = 0;
    int cnt_pos = 0;
    char curr_char;
    // read until find 5 -n value cnt or no char left

	while (cnt_new_line < 5) {
        n = read(fd, curr_char, 1);
        lseek(fd, cnt_pos, SEEK_END);
        if (curr_char == 10) {
            cnt_new_line++;
        }
        cnt_pos --;
    }

    for (int i=0; i<cnt_new_line; i++) {
        n = write(STDIN_FILENO, buf, BUFSIZ);
    }

    if (close(fd) < 0)
        err("close1");
}