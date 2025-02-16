#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <time.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define MAXLINE 1024
#define LISTENQ 10

void str_echo( int connfd)
{
	int n;
	char line[MAXLINE];

	while ( (n = read(connfd, line, MAXLINE)) > 0) { 
		line[n] = 0;
		if ( write(connfd, line, strlen(line)) != strlen(line))
			err("write")		
	}
}

int main(int argc, char *argv[])
{
	int n;
	int listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	char buf[MAXLINE];
	time_t ticks;
	socklen_t len;

	if ( argc != 2) {
		printf("Uporaba: %s Port\n", argv[0]);
		exit(0);
	}
	unsigned short Port = atoi(argv[1]);

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if ( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		err("bind")	
	
	if ( listen(listenfd, LISTENQ) < 0)
		err("listen")

	while (1) {	
		//if ( (connfd = accept(listenfd, NULL, NULL)) < 0)
		if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
			err("accept")

		str_echo(connfd);

		if ( close(connfd) < 0)
			err("close")
	}

}