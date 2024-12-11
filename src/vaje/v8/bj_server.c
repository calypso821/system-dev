#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define MAXLINE 1024
#define LISTENQ 10
#define SA struct sockaddr

#define err(mess)   { fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

int main(int argc, char **argv)
{
    int n;
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];
    time_t ticks;
    socklen_t len;

    if ( argc != 2) {
        printf("Uporaba: %s TCP_vrata\n", argv[0]);
        exit(0);
    }
    unsigned short Port = atoi(argv[1]);

    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        err("socket")

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(Port);
    if ( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err("bind") 

    if ( listen(listenfd, LISTENQ) < 0)
        err("listen")

    while (1) {
        //if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0) 
        if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
            err("accept")

        // Kdo je odjemalec?
        if ( inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)) < 0)
            err("ntop") 
        printf("Odjemalec je %s, port %d\n", buf, ntohs(cliaddr.sin_port));

        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        if ( write(connfd, buf, strlen(buf)) != strlen(buf))
            err("write")
            
        if ( close(connfd) < 0)
            err("close")
    }
}