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
    // Get access to shared memory
    key_t shm_key = ftok("/tmp", 'M');
    int shmid = shmget(shm_key, sizeof(struct shared_board), 0644);
    if (shmid < 0) err("shmget");

    struct shared_board *board = shmat(shmid, NULL, 0);
    if (board == (void *)-1) err("shmat");

    // Get access to semaphore
    key_t sem_key = ftok("/tmp", 'S');
    int semid = semget(sem_key, 1, 0644);
    if (semid < 0) err("semget");

    // Get access to message queue
    key_t msg_key = ftok("/tmp", 'Q');
    int msgid = msgget(msg_key, 0644);
    if (msgid < 0) err("msgget");

    // Set up semaphore operation
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    operations[0].sem_flg = 0;

    // Message queue buffer
    struct msg_buf notification;

    printf("Waiting for messages...\n");
    int last_count = 0;

    while (1) {
        // Wait for notification
        if (msgrcv(msgid, &notification, sizeof(notification.notice), 1, 0) < 0)
            err("msgrcv");

        // Lock the board
        operations[0].sem_op = -1;
        if (semop(semid, operations, 1) < 0) err("semop lock");

        // Read message if it's new
        if (board->msg_count > last_count) {
            printf("Message #%d: %s\n", board->msg_count, board->message);
            last_count = board->msg_count;
        }

        // Unlock the board
        operations[0].sem_op = 1;
        if (semop(semid, operations, 1) < 0) err("semop unlock");
    }

    // Cleanup (this part won't be reached in this simple version)
    if (shmdt(board) < 0) err("shmdt");

    return 0;
}