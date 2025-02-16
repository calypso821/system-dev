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

int draw(int *deck, int *top)
{
    return deck[(*top)--];
}

int get_score(int card)
{
    if (card > 10)
        return 10;
    return card;
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
    char msg[MAXLINE];

    // Initial cards for player
    int card1 = draw(deck, &top);
    int card2 = draw(deck, &top);
    player_score = get_score(card1) + get_score(card2);

    // Initial cards for dealer (one hidden)
    int dealer_card1 = draw(deck, &top);
    int dealer_card2 = draw(deck, &top);
    dealer_score = get_score(dealer_card1) + get_score(dealer_card2);

    // Send initial cards and score to player: "card1 card2 score dealer_card1"
    snprintf(msg, MAXLINE, "%d %d %d %d", 
             card1, card2, player_score, dealer_card1);
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
            player_score += get_score(new_card);

            snprintf(msg, MAXLINE, "%d %d", new_card, player_score);
            write(connfd, msg, strlen(msg));
            
            if (player_score > 21) {
                break;
            }

        }
        else if (choice[0] == 's') {
            // First send dealer's initial cards
            snprintf(msg, MAXLINE, "%d %d %d", 
                    dealer_card1, dealer_card2, dealer_score);
            if (write(connfd, msg, strlen(msg)) != strlen(msg)) {
                err("write");
            }
            usleep(100000);  // 100ms delay

            // Dealer draws until 17 or higher
            while (dealer_score < 17) {
                int new_card = draw(deck, &top);
                dealer_score += get_score(new_card);
                // Send each dealer draw separately
                snprintf(msg, MAXLINE, "%d %d", new_card, dealer_score);
                if (write(connfd, msg, strlen(msg)) != strlen(msg)) {
                    err("write");
                }
                usleep(100000);  // 100ms delay
            }
            break;
        }
    }
    // Game finished
    // Calculate result and send final message
    const char *result;
    if (player_score > 21) {
        result = "LOSE";  // Player bust
    } else if (dealer_score > 21) {
        result = "WIN";   // Dealer bust
    } else if (dealer_score > player_score) {
        result = "LOSE";  // Dealer wins
    } else if (dealer_score < player_score) {
        result = "WIN";   // Player wins
    } else {
        result = "TIE";   // Equal scores
    }

    printf("Dealer score: %d\n", dealer_score);
    printf("Player score: %d\n", player_score);
    printf("Res: %s\n", result);
    
    // Send end signal before final result
    snprintf(msg, MAXLINE, "END");
    write(connfd, msg, strlen(msg));
    usleep(100000);

    // Send final result
    snprintf(msg, MAXLINE, "%d %d %s", player_score, dealer_score, result);
    write(connfd, msg, strlen(msg));
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