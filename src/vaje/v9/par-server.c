#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define err(mess) { fprintf(stderr, "Error: %s\n", mess); exit(1); }
#define MAXLINE 1024
#define LISTENQ 10

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}

void str_echo(int connfd, struct sockaddr_in cliaddr) {
    int n;
    char line[MAXLINE];
    char response[MAXLINE];
    
    // Read first message containing thread number
    if ((n = read(connfd, line, MAXLINE)) > 0) {
        line[n] = 0;
        int thread_num = atoi(line);
        pid_t my_pid = getpid();
        
        printf("I am child %d serving client thread %d from %s:%d\n", 
               my_pid, 
               thread_num,
               inet_ntoa(cliaddr.sin_addr),
               ntohs(cliaddr.sin_port));
               
        // Check win condition and send response
        if (my_pid % thread_num == 0) {
            snprintf(response, sizeof(response), "Victory you won! (PID: %d %% TID: %d == 0)", my_pid, thread_num);
        } else {
            snprintf(response, sizeof(response), "You lost! (PID: %d %% TID: %d != 0)", my_pid, thread_num);
        }
        write(connfd, response, strlen(response));
    }
}

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    pid_t pid;

    if (argc != 2) {
        printf("Usage: %s Port\n", argv[0]);
        exit(1);
    }

    unsigned short port = atoi(argv[1]);
    signal(SIGCHLD, sig_chld);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err("bind");

    if (listen(listenfd, LISTENQ) < 0)
        err("listen");

    printf("Server listening on port %d\n", port);

    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
            err("accept");
        }

        if ((pid = fork()) < 0) {
            err("fork");
        } 
        else if (pid == 0) {  // Child
            close(listenfd); // Close listening sokcet
            str_echo(connfd, cliaddr); // Process communication
            exit(0); // This exit triggers SIGCHLD
        }
        close(connfd); // Parent closes connection socket (child handling it)
    }
    return 0;
}