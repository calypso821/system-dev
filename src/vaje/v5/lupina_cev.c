#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define err_sys(mess) { fprintf(stderr,"Error: %s.", mess); exit(1); }

#define BUFFSIZE 1024
#define MAX_ARGS 64

void parse(char *command_str, char *args[]);
void parse_strtok(char* command_str, char *args[]);
bool check_if_pipe(char *args[], char ***left, char ***right);

void sig_handler(int signo) {
    if (signo == SIGINT)
        printf("\nCaught SIGINT (signal %d)\n", signo);
    else if (signo == SIGUSR1)
        printf("\nCaught SIGUSR1 (signal %d)\n", signo);
    exit(0);
}
// ps -ef => grep --color=always root
// Grep disable coloring when ouput to pipe
// Force enable

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

    int pfd[2];
    char pipe_buf[512];

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

        char **left, **right;
        bool pipe_status = check_if_pipe(args, &left, &right);
        
        // Create child for command execute
        if ((pid = fork()) < 0)
            err_sys("fork error")
        else if (pid == 0) { // child 1
            if (pipe_status)
            {
                // Create pipe first if needed
                if (pipe(pfd) == -1) {
                    err_sys("pipe error");
                }

                pid_t pid2;

                // Create second child
               if ((pid2 = fork()) < 0) {
                    err_sys("fork error");
                }
                else if (pid2 == 0) { // child 2
                    //sleep(1);
                    // Configure for reading from pipe
                    close(pfd[1]);  // Close write end
                    if (dup2(pfd[0], STDIN_FILENO) == -1) {
                        err_sys("dup2 error (child2)");
                    }
                    close(pfd[0]);  // Close after dup2

                    // Execute right side 
                    execvp(right[0], right);
                    printf("couldn't execute: %s\n", buf);
                    exit(127);
                }

                // Child 1 
                //Configure for writing to pipe
                close(pfd[0]);  // Close read end
                if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                    err_sys("dup2 error (child1)");
                }
                close(pfd[1]);  // Close after dup2;
            }

            // Execute left side 
            execvp(left[0], left);
            printf("couldn't execute: %s\n", buf);
            exit(127);
 
        }
        // parent - wait for child 1 to finish
        if ((pid = waitpid(pid, &status, 0)) < 0)
            err_sys("waitpid error")

        // Next command
        command_num = (command_num + 1) % 2;
        printf("%% ");
    }
} 

bool check_if_pipe(char *args[], char ***left, char ***right)
{
    // Set **left to where args points
    *left = args;
    while (*args != NULL)
    {
        if (strcmp(*args, "=>") == 0)
        {
            *args = NULL;
            // Set **right to where args + 1 points
            *right = args + 1;
            return true;
        }
        args++;
    }
    return false;
}

void parse(char *command_str, char *args[])
{
    int nargs = 0;

    while (*command_str != '\0' && nargs < MAX_ARGS - 1)
    {
        // Reapcle all empty with '\0'
        while (*command_str == ' ' || *command_str == '\t')
        {
            *command_str++ = '\0';
        }
        // Mark new arg
        if (*command_str != 0)
        {
            *args++ = command_str;
            nargs++;
        }
        // increment until end of arg
        while (*command_str != ' ' &&
                *command_str != '\t' && 
                *command_str != '\0')
            {
                command_str++;
            }
    }
    // Change last arg pointer to be null
    // *args = (char *)0;
    *args = NULL;
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