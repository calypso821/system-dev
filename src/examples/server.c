#include <stdio.h>      // Standard I/O operations
#include <stdlib.h>     // Standard library functions
#include <string.h>     // String manipulation functions
#include <unistd.h>     // UNIX standard functions
#include <stdbool.h>    // Boolean type and values
#include <err.h>        // Error handling
#include <sys/socket.h> // Socket operations
#include <netinet/in.h> // Internet address family
#include <arpa/inet.h>  // Internet operations
#include <time.h>       // Time functions

// Compilation and execution instructions
// cd /data/repos/fri/spo/src
// gcc -o server server.c
// ./server

// Constants
#define MAXLINE 1024   // Maximum line length for reading
#define BUFFSIZE 254   // Buffer size for file operations
#define LISTENQ 10     // Maximum length of queue of pending connections
#define err(mess) { fprintf(stderr, "Napaka: %s\n", mess); exit(0); }

// Searches for a string in the configuration file

char* look_for_str(char *look_str)
{
    char *filepath = "/etc/adduser.conf";
    static char result[BUFFSIZE];    // Static buffer for return string
    char buf[BUFFSIZE];
    FILE *fp;

    // Open the configuration file
    if ((fp = fopen(filepath, "r")) == NULL) {
        return "Error opening file\n";
    }

    // Initialize counters and flags
    int line_cnt = 1;
    bool found = false;
    int matches[MAXLINE];           // Array to store matching line numbers
    int match_count = 0;

    // Read file line by line
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (strstr(buf, look_str) != NULL) {
            matches[match_count++] = line_cnt;
            found = true;
        }
        line_cnt++;
    }
    fclose(fp);

    // If string wasn't found, return "0\n"
    if (!found) {
        return "0\n";
    }

    // Build result string with line numbers separated by commas
    result[0] = '\0';  // Initialize empty string
    for (int i = 0; i < match_count; i++) {
        char temp[20];  // Temporary buffer for number and comma
        sprintf(temp, "%d%s", matches[i],
                (i < match_count - 1) ? ", " : "\n");
        strcat(result, temp);
    }
    return result;
}


// Handles communication with a connected client
// Reads search strings from client and sends back results

void process_request(int connfd)
{
    int n;
    char line[MAXLINE];

    // Read and process client requests until connection closes
    while ((n = read(connfd, line, MAXLINE)) > 0) {
        line[n] = '\0';  // Null terminate received string
        
        // Remove trailing newline if present
        if (line[n-1] == '\n') {
            line[n-1] = '\0';
            n--;
        }

        printf("Looking for string: %s\n", line);
        char *found = look_for_str(line);
        printf("Found in lines: %s", found);

        // Send result back to client
        size_t len = strlen(found);
        if (write(connfd, found, len) != len) {
            err("write");
        }
    }

    // Check for read error
    if (n < 0) {
        err("read");
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    unsigned short Port = 45563;

    // Create TCP socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("socket")

    // Set socket option to reuse address
    int optval = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        err("setsockopt");

    // Initialize server address structure
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(Port);           // Convert to network byte order
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on any interface

    // Bind socket to address
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err("bind")

    // Start listening for connections
    if (listen(listenfd, LISTENQ) < 0)
        err("listen")

    printf("Server listening...\n");
    fflush(stdout);

    // Main server loop
    while (1) {
        // Accept new client connection
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
            err("accept")

        printf("Client accepted!\n");
        fflush(stdout);

        // Process client requests
        process_request(connfd);

        // Close connection
        if (close(connfd) < 0)
            err("close")
    }
}