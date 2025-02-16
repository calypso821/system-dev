#include <unistd.h>     // UNIX standard functions (read, write, close)
#include <stdio.h>      // Standard I/O operations
#include <stdlib.h>     // Standard library functions
#include <sys/socket.h> // Socket operations
#include <netinet/in.h> // Internet address family
#include <arpa/inet.h>  // Internet operations (inet_pton)
#include <string.h>     // String manipulation functions
#include <strings.h>    // BSD string functions (bzero)

// Error handling macro
#define err(mess) { fprintf(stderr, "Napaka: %s\n", mess); exit(0); }

// Maximum buffer size for reading/writing
#define MAXLINE 1024


// Handles the client-side communication with server
// Reads input from user and displays server responses

void str_cli(int sockfd)
{
    int n;
    char sendline[MAXLINE];    // Buffer for sending data to server
    char recvline[MAXLINE];    // Buffer for receiving server responses

    // Display initial prompt
    printf("Enter string to search: ");
    fflush(stdout);  // Ensure prompt is displayed immediately

    // Main communication loop
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        // Send user input to server
        if (write(sockfd, sendline, strlen(sendline)) != strlen(sendline))
            err("write")

        // Read server's response
        n = read(sockfd, recvline, MAXLINE);
        if (n < 0)
            err("read")
        else if (n == 0)
            err("read, server koncal.")  // Server closed connection

        // Null-terminate and display received data
        recvline[n] = 0;
        printf("%s", recvline);

        // Prompt for next input
        printf("\nEnter string to search: ");
        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
    int sockfd;                     // Socket file descriptor
    struct sockaddr_in servaddr;    // Server address structure
    char buf[MAXLINE];
    unsigned short Port = 45563;    // Server port number

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("socket")

    // Initialize server address structure
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  // IPv4
    servaddr.sin_port = htons(Port);  // Convert port to network byte order

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr) <= 0)
        err("pton")

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err("connect")

    // Enter interactive mode
    str_cli(sockfd);

    // Clean up: close socket
    if (close(sockfd) < 0)
        err("close")
}