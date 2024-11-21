#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define err_sys(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

#define BUFFSIZE 1024
#define MAX_ARGS 64

void parse(char *command_str, char *args[])
{
    int nargs = 0;
    // Initial pointer (first argument)
    args[nargs] = command_str;

    while (*command_str != '\0' && nargs < MAX_ARGS - 1)
    {
        if (*command_str == ' ')
        {
            // New argument
            *command_str = '\0';
            // Save pointer of next argument
            args[++nargs] = command_str + 1;
        }
        // Increse command_str index
        command_str++; 
    }
    // Change last arg to be null pointer
    args[++nargs] = NULL;
}

void parse_strtok(char* command_str, char *args[])
{
    int nargs = 0;
    char *token = strtok(command_str, " ");
    while (token != NULL)
    {
        args[nargs++] = token;
        token = strtok(NULL, " ");
    }
    args[nargs] = NULL;
}

void sig_handler(int signo) {
    if (signo == SIGINT)
        printf("\nCaught SIGINT (signal %d)\n", signo);
    else if (signo == SIGUSR1)
        printf("\nCaught SIGUSR1 (signal %d)\n", signo);
    exit(0);
}

int main(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        err_sys("can't catch SIGINT");
    }
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        err_sys("can't catch SIGUSR1")
    }

    printf("Process ID: %d\n", getpid());
    //kill -USR1 id

    char buf[BUFFSIZE];
    char *args[MAX_ARGS];

    pid_t pid;
    int status;
    int command_num = 1;

    printf("%% "); // izpiše prompt %
    while (fgets(buf, BUFFSIZE, stdin) != NULL)
    {
        // Remove newline 
        buf[strlen(buf) - 1] = '\0';

        if (command_num % 2 == 1) {
            // Odd commnad
            parse(buf, args);
        }
        else {
            // Even command
            parse_strtok(buf, args);
        }


        // Create child for command execute
        if ( (pid = fork()) < 0)
            err_sys("fork error")
        else if (pid == 0) { // child
            execvp(args[0], args);
            printf("couldn't execute: %s\n", buf);
            exit(127);
        }
        // starš - wait for child to finish
        if ( (pid = waitpid(pid, &status, 0)) < 0)
            err_sys("waitpid error")

        // Next command
        command_num = (command_num + 1) % 2;
        printf("%% ");
    }
} 