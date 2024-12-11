#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>

#define err(mess) { printf("Error: %s\n", mess); exit(1); }

#define MAXLINE 1024

void str_cli( int sockfd)
{
	int n;
	char sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		if ( write(sockfd, sendline, strlen(sendline)) != strlen(sendline))
			err("write")
		n = read(sockfd, recvline, MAXLINE);
		if ( n < 0)
			err("read")
		else if ( n == 0)
			err("read, server koncal.")			
		recvline[n] = 0;
		puts(recvline);
	}
}

int main(int argc, char **argv)
{
	int i, sockfd[100];
	struct sockaddr_in servaddr;
	
	if ( argc != 3) {
		printf("Uporaba: %s Port N\n", argv[0]);
		exit(0);
	}
	unsigned short Port = atoi(argv[1]);
	int N = atoi(argv[2]);
	
	for (i = 0; i < N; i++) {
		if (( sockfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			err("socket error")
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(Port);
		if ( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0)
			err("inet_pton")
		if ( connect(sockfd[i], (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
			err("connect")
	}
	str_cli(sockfd[0]);
	exit(0);
}