#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256


int main(int argc, char* argv[]) {
    char buf[BUFSIZE];
    int fd, n, read_size;

    char *path;

    char char1 = *argv[1]; // same as argv[1][0]
    char char2 = *(argv[1] + 1); // same as argv[1][1]

    if (argc == 2) {
        // Mode 1: default n (5)
        path = argv[1]; 
        n = 5;
    } else if (argc == 4 && *argv[1] == '-' && *(argv[1] + 1) == 'n') {
        // Mode 2: input n
        path = argv[3];
        n = atoi(argv[2]);
    } else {
        printf("Usage: %s [-n value] file\n", argv[0]);
        return 0;
    }

    if ((fd = open(path, O_RDONLY)) < 0) {
        perror("Error opening file");
        return 1;
    }

    // Move to the end of file (-1 = last char)
    int pos = lseek(fd, -1, SEEK_END);
    int cnt_new_line = 0;
    char curr_char;

    // Finde last n lines 
    // read until find n + 1 * of new lines or no char left
	while (pos >= 0) {
        // Read char + lseek curr_pos +1
        if (read(fd, &curr_char, 1) != 1) {
            perror("Error reading file");
            return 1;
        }
        if (curr_char == 10) {
            cnt_new_line++;
            if (cnt_new_line == n + 1) {
                break;
            }
        }
        pos = lseek(fd, -2, SEEK_CUR);
    }

    // Last char = new line, +1
   pos = lseek(fd, pos+1, SEEK_SET);
   // Write last n lines to stdout
    while ((read_size = read(fd, buf, BUFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, read_size) != read_size) {
            perror("Error writing to stdout");
            return 1;
        }
    }
		
	if (read_size < 0)
		perror("read error");
        return 1;

    if (close(fd) < 0)
        perror("close error");
        return 1;

    return 0;
}
