#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 256
#define PROMPT { printf("Enter text (Ctrl+D to end): "); fflush(stdout); }

int main(int argc, char* argv[]) {
    char buf[BUFSIZE];
    int fd, n;

    if (argc == 2) {
        // Mode 1: Write to file (set to 0)
        fd = open(argv[1], O_TRUNC | O_WRONLY);
    } else if (argc == 3 && *argv[1] == '-' && *(argv[1] + 1) == 'a') {
        // Mode 2: Append to file
        fd = open(argv[2], O_APPEND | O_WRONLY);
    } else {
        printf("Usage: %s [-a] file\n", argv[0]);
        return 0;
    }

    PROMPT;
    while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
        // Write to standard ouput
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("Error writing to stdout");
            return 1;
        }
        // Write to output file
        if (write(fd, buf, n) != n) {
            perror("Error writing to output file");
            return 1;
        }
        PROMPT;
        fflush(stdout);
    }

    // Correct new line terminal
    printf("\n");

	if (n < 0)
		perror("read error");
        return 1;

    if (close(fd) < 0)
        perror("close error");
        return 1;

    return 0;
}