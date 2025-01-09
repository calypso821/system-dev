#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define err(mess) { fprintf(stderr, "Error: %s\n", mess); exit(1); }

struct thread_args {
    int thread_num;
    int server_port;
};


void *client_thread(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    //printf("Thread %d created, sending ID to server\n", args->thread_num);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(args->server_port);
    
    // Connect to local host
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
        err("inet_pton");

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err("connect");

    // Send thread number
    snprintf(sendline, sizeof(sendline), "%d", args->thread_num);
    write(sockfd, sendline, strlen(sendline));
    
    // receive win/lose response
    int n = read(sockfd, recvline, MAXLINE);
    recvline[n] = 0;
    printf("Thread %d result: %s\n", args->thread_num, recvline);

    close(sockfd); // Close socket
    return NULL; // End of thread
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Port> <Number of threads>\n", argv[0]);
        exit(1);
    }

    int num_threads = atoi(argv[2]);
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    struct thread_args *args = malloc(num_threads * sizeof(struct thread_args));

    for (int i = 0; i < num_threads; i++) {
        args[i].thread_num = i + 1;
        args[i].server_port = atoi(argv[1]);
        
        // Create new thread for each client connection
        if (pthread_create(&threads[i], NULL, client_thread, &args[i]) != 0)
            err("pthread_create");
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(args);
    return 0;
}