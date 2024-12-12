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

#define DECK_SIZE 52

char get_card_char(int card)
{
    switch (card) {
        case 1:  return 'A';
        case 10: return '10';
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

int draw(int *deck, int *top)
{
    return deck[(*top)--];
}

void create_deck(int *deck)
{
    int pos = 0;
    // 4 sets of cards
    for (int set = 0; set < 4; set++) {
        // 1 -> 13
        for (int i = 1; i < 14; i++) {
            deck[pos] = i;
            pos++;
        }
    }
}

void shuffle_deck(int *deck)
{
    srand(time(NULL)); // set seed
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap cards[i] and cards[j]
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void play_bj(int connfd)
{
    int deck[DECK_SIZE];
    create_deck(deck);
    shuffle_deck(deck);
    int top = DECK_SIZE - 1;
    
    int player_score = 0;
    int dealer_score = 0;
    char cards[3];
    char msg[MAXLINE];

    // Initial cards for player
    int card1 = draw(deck, &top);
    int card2 = draw(deck, &top);
    player_score = card1 + card2;

    // Initial cards for dealer (one hidden)
    int dealer_card1 = draw(deck, &top);
    int dealer_card2 = draw(deck, &top);
    dealer_score = dealer_card1 + dealer_card2;
    
    // Send initial cards and score to player
    snprintf(msg, MAXLINE, "%c%c %d (Dealer shows: %c)", 
             get_card_char(card1), get_card_char(card2), 
             player_score, get_card_char(dealer_card1));
    if (write(connfd, msg, strlen(msg)) != strlen(msg)) {
        err("write");
    }

    // Player's turn
    char choice[MAXLINE];
    int n;
    int game_over = 0;
    
    while ((n = read(connfd, choice, MAXLINE)) > 0) {
        choice[n] = '\0';
        
        if (choice[0] == 'h') {
            int new_card = draw(deck, &top);
            player_score += new_card;
            snprintf(msg, MAXLINE, "%c %d", get_card_char(new_card), player_score);
            if (write(connfd, msg, strlen(msg)) != strlen(msg)) {
                err("write");
            }
            
            if (player_score > 21) {
                snprintf(msg, MAXLINE, "BUST! Dealer wins!");
                write(connfd, msg, strlen(msg));
                game_over = 1;
                break;
            }
        }
        else if (choice[0] == 's') {
            // Dealer's turn
            snprintf(msg, MAXLINE, "Your final score: %d. Dealer's cards: %c%c (%d)", 
                     player_score, get_card_char(dealer_card1), 
                     get_card_char(dealer_card2), dealer_score);
            write(connfd, msg, strlen(msg));
            
            // Dealer draws until 17 or higher
            while (dealer_score < 17) {
                int new_card = draw(deck, &top);
                dealer_score += new_card;
                snprintf(msg, MAXLINE, "\nDealer draws: %c (Total: %d)", 
                         get_card_char(new_card), dealer_score);
                write(connfd, msg, strlen(msg));
            }
            
            // Determine winner
            if (dealer_score > 21) {
                snprintf(msg, MAXLINE, "\nDealer BUST! You win!");
            } else if (dealer_score > player_score) {
                snprintf(msg, MAXLINE, "\nDealer wins with %d!", dealer_score);
            } else if (dealer_score < player_score) {
                snprintf(msg, MAXLINE, "\nYou win with %d!", player_score);
            } else {
                snprintf(msg, MAXLINE, "\nIt's a tie at %d!", player_score);
            }
            write(connfd, msg, strlen(msg));
            break;
        }
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

	if (argc != 2) {
		printf("Uporaba: %s Port\n", argv[0]);
		exit(0);
	}
	unsigned short Port = atoi(argv[1]);

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err("socket")
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(Port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		err("bind")	
	
	if (listen(listenfd, LISTENQ) < 0)
		err("listen")

	while (1) {	
		//if ( (connfd = accept(listenfd, NULL, NULL)) < 0)
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0)
			err("accept")

		play_bj(connfd);

		if (close(connfd) < 0)
			err("close")
	}

}