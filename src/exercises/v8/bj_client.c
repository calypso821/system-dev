#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

#define MAXLINE 1024

char get_card_char(int card)
{
    switch (card) {
        case 1:  return 'A';
        case 10: return 'T';
        case 11: return 'J';
        case 12: return 'Q';
        case 13: return 'K';
        default:
            // For cards 2-10, convert number to ASCII char
            // '0' -> 48 + 1 = 49 '1';
            if (card >= 2 && card <= 9)
                return '0' + card;
            return '?';  // Invalid card
    }
}

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

    // Parse initial cards: "card1 card2 score dealer_card1"
    int card1, card2, score, dealer_card;
    sscanf(recvline, "%d %d %d %d", &card1, &card2, &score, &dealer_card);
    printf("Your cards: %c, %c (Score: %d) (Dealer shows: %c)\n",
           get_card_char(card1), get_card_char(card2),
           score, get_card_char(dealer_card));

    // Game loop
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        if (sendline[0] != 'h' && sendline[0] != 's') {
            printf("Invalid input. Use 'h' for hit or 's' for stand\n");
            continue;
        }

        if (write(sockfd, sendline, strlen(sendline)) != strlen(sendline)) {
            err("write");
        }

        if (sendline[0] == 'h') {
            n = read(sockfd, recvline, MAXLINE);
            if (n <= 0) err("read");
            recvline[n] = 0;

            int new_card, new_score;
            sscanf(recvline, "%d %d", &new_card, &new_score);
            printf("New card: %c (Score: %d)\n", get_card_char(new_card), new_score);
            if (new_score > 21) {
                printf("BUST! Dealer wins! (Yout score: %d)\n", new_score);
                break;
            }
        }
        else if (sendline[0] == 's') {
            // Read dealer's cards
            n = read(sockfd, recvline, MAXLINE);
            if (n <= 0) {
                printf("Error reading dealer cards: %d\n", n);  // More debug info
                err("read dealer cards");
            }
            recvline[n] = 0;

            int d_card1, d_card2, d_score;
            if (sscanf(recvline, "%d %d %d", &d_card1, &d_card2, &d_score) != 3) {
                err("parse dealer cards");
            }
            printf("Dealer's cards: %c, %c (Score: %d)\n",
                get_card_char(d_card1), get_card_char(d_card2), d_score);

            int new_card, new_score;

            // Read dealer's draws
            while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
                recvline[n] = 0;

                if (strcmp(recvline, "END") == 0) break;

                sscanf(recvline, "%d %d", &new_card, &new_score);
                printf("Dealer draws: %c (Total: %d)\n", 
                    get_card_char(new_card), new_score);
            }

            int player_score, dealer_score;
            char status[10];


            read(sockfd, recvline, MAXLINE);
            sscanf(recvline, "%d %d %s", &player_score, &dealer_score, status);

            // printf("Dealer score: %d\n", dealer_score);
            // printf("Player score: %d\n", player_score);
            // printf("Res: %s\n", status);


            if (strcmp(status, "WIN") == 0) {
                printf("You win with %d! (Dealer: %d)\n", player_score, dealer_score);
            } else if (strcmp(status, "LOSE") == 0) {
                printf("Dealer wins with %d! (You: %d)\n", dealer_score, player_score);
            } else if (strcmp(status, "TIE") == 0)  {  // TIE
                printf("It's a tie at %d!\n", player_score);
            }
            break;
        }
    }
    printf("Game finished!\n");
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