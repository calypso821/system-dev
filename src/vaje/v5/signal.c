#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define err_sys(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

// Phase 1: Show unslept time
void sig_handler1(int signo) {
    if (signo == SIGINT) {
        // Cancle alarm
        int unslept_time = alarm(0);
        printf("\nunslept_time = %d\n", unslept_time);
        alarm(unslept_time);
    }
}

// Phase 2: Ignore SIGINT
void sig_handler2(int signo) {
    // This won't actually be called since we'll use SIG_IGN
}

// Phase 3: Default action (terminate)
// SIG_DFL

void alarm_handler(int signo) {
    static int phase = 1;
    
    printf("\nSetting alarm for another 5 seconds...\n");
    alarm(5);
    
    phase++;
    if (phase == 2) {
        printf("=== Phase 2: SIGINT will be ignored ===\n");
        signal(SIGINT, sig_handler2);
    }
    else if (phase == 3) {
        printf("=== Phase 3: SIGINT will terminate program ===\n");
        signal(SIGINT, SIG_DFL);
    }
    else {
        exit(0);
    }
}

int main(void)  
{
    if (signal(SIGINT, sig_handler1) == SIG_ERR) {
        err_sys("can't catch SIGINT");
    }
        if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        err_sys("can't catch SIGALRM");
    }

    printf("Process ID: %d\n", getpid());
    
    printf("=== Phase 1: SIGINT will show unslept time ===\n");
    printf("Setting alarm for 5 seconds...\n");
    alarm(5);
    
    int cnt = 1;
    while(1) {
        sleep(1);
        printf("Time %d\n", cnt++);
    }
    
    return 0;
}