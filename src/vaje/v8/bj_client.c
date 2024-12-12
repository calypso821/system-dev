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

void play_bj(int sockfd)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    // Get initial cards
    n = read(sockfd, recvline, MAXLINE);
    if (n <= 0) {
        err("read initial cards");
    }
    recvline[n] = 0;
    printf("Your cards: %s\n", recvline);
    
    // Game loop
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        if (sendline[0] != 'h' && sendline[0] != 's') {
            printf("Invalid input. Use 'h' for hit or 's' for stand\n");
            continue;
        }
        
        if (write(sockfd, sendline, strlen(sendline)) != strlen(sendline)) {
            err("write");
        }
        
        n = read(sockfd, recvline, MAXLINE);
        if (n <= 0) {
            if (sendline[0] == 's') {
                printf("Game over!\n");
                break;
            }
            err("read");
        }
        recvline[n] = 0;
        printf("%s\n", recvline);
        
        // Check if game should end
        if (strstr(recvline, "BUST!") || sendline[0] == 's') {
            break;
        }
    }
}

int main( int argc, char *argv[])
{
	int n;
	int sockfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];

	if (argc != 2) {
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
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		err("connect")
	
	play_bj(sockfd);
	
	if (close(sockfd) < 0)
		err("close")

}