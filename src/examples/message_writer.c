#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define err(mess) { fprintf(stderr, "Error: %s\n", mess); exit(1); }
#define MAX_MSG 100

// Shared Memory (struct shared_board):
// Stores the current message
// Keeps track of message count
// Shared between writer and reader


// Semaphore:
// Controls access to shared memory
// Ensures writer and reader don't conflict
// Simple binary semaphore (mutex)


// Message Queue:
// Notifies reader when new message is available
// Simple notification system

// How it works:
// Writer:

// User enters messages
// Locks shared memory
// Writes message to board
// Unlocks shared memory
// Sends notification via message queue


// Reader:
// Waits for notifications
// When notified, locks shared memory
// Reads and displays new message
// Unlocks shared memory

// Structure for shared memory
struct shared_board {
    char message[MAX_MSG];
    int msg_count;
};

// Structure for message queue notification
struct msg_buf {
    long mtype;
    char notice[20];
};

// Semaphore union
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    // Initialize shared memory
    key_t shm_key = ftok("/tmp", 'M');
    int shmid = shmget(shm_key, sizeof(struct shared_board), 0644 | IPC_CREAT);
    if (shmid < 0) err("shmget");

    // Attach shared memory
    struct shared_board *board = shmat(shmid, NULL, 0);
    if (board == (void *)-1) err("shmat");
    board->msg_count = 0;

    // Initialize semaphore
    key_t sem_key = ftok("/tmp", 'S');
    int semid = semget(sem_key, 1, 0644 | IPC_CREAT);
    if (semid < 0) err("semget");

    // Set initial semaphore value
    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) < 0) err("semctl");

    // Initialize message queue
    key_t msg_key = ftok("/tmp", 'Q');
    int msgid = msgget(msg_key, 0644 | IPC_CREAT);
    if (msgid < 0) err("msgget");

    // Set up semaphore operation
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_flg = 0;

    // Set up message notification
    struct msg_buf notification;
    notification.mtype = 1;
    strcpy(notification.notice, "New message!");

    char input[MAX_MSG];
    printf("Enter messages (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fgets(input, MAX_MSG, stdin);
        input[strcspn(input, "\n")] = 0;  // Remove newline

        if (strcmp(input, "exit") == 0) break;

        // Lock the board
        operations[0].sem_op = -1;
        if (semop(semid, operations, 1) < 0) err("semop lock");

        // Write message
        strcpy(board->message, input);
        board->msg_count++;
        printf("Message posted: %s\n", input);

        // Unlock the board
        operations[0].sem_op = 1;
        if (semop(semid, operations, 1) < 0) err("semop unlock");

        // Notify reader
        if (msgsnd(msgid, &notification, sizeof(notification.notice), 0) < 0) 
            err("msgsnd");
    }

    // Cleanup
    if (shmdt(board) < 0) err("shmdt");
    if (shmctl(shmid, IPC_RMID, NULL) < 0) err("shmctl");
    if (semctl(semid, 0, IPC_RMID) < 0) err("semctl");
    if (msgctl(msgid, IPC_RMID, NULL) < 0) err("msgctl");

    return 0;
}