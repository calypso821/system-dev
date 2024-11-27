#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define err(mess)	{ fprintf( stderr, "Napaka: %s\n", mess); exit(0); }

static void sig_alrm(int signo)
{
	return; // da zbudi pavzo
}

static void sig_usr1(int signo)
{
	printf("Prejel sem SIGUSR1.\n");
}

unsigned int sleep1(unsigned int nsecs)
{
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		return(nsecs);
	
	alarm(nsecs); // vklop časovnika
	pause(); // naslednji ujeti signal nas zbudi
	
	return( alarm(0) ); // izklop časovnika, vrne neprespani čas
}

int main(int argc, char *argv[])
{
	int sekund;

	if ( argc != 2) 
		err("argc != 2")

	sekund = atoi(argv[1]);

	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
		return(sekund);

	printf("%d\n", sleep1(sekund));
	
	return 0;
}