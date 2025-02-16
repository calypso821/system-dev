#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

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

int main( int argc, char *argv[])
{
	int n;
	int sockfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];

	if ( argc != 2) {
		printf("Uporaba: %s Port\n", argv[0]);
		exit(0);
	}
	unsigned short Port = atoi(argv[1]);

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);
	if ( inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr) <= 0)
		err("pton")
	if ( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		err("connect")
	
	str_cli(sockfd);
	
	if ( close(sockfd) < 0)
		err("close")

}