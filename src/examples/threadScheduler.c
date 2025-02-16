#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// Error handling macro - prints error message and exits
#define err(mess) { fprintf(stderr, "Napaka: %s\n", mess); exit(1); }

// Union required for semctl operations
// Different members allow setting different semaphore parameters
union semun {
    int val;                // Value for SETVAL
    struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET
    unsigned short *array;  // Array for GETALL, SETALL
};

// Global variables - accessible by all threads
key_t sem_key;             // Key for semaphore identification
int semid;                 // Semaphore ID
time_t start_time;         // Program start time
struct sembuf operation[1]; // Semaphore operations structure

// Thread function - executed by each thread
void* thread_function(void* arg) {
    // Convert void pointer argument back to thread ID
    long id = (long)arg;
    time_t current_time;
    
    // CRITICAL SECTION BEGIN - Request access
    // Decrement semaphore (lock)
    operation[0].sem_op = -1;
    semop(semid, operation, 1);
    printf("Memory: locked by thread %ld\n", id);
    
    // Get and print current time relative to program start
    current_time = time(NULL);
    printf("Thread %ld starts. (t=%ld)\n", id, current_time - start_time);
    
    // Simulate work with sleep
    sleep(3);
    
    // Print completion time
    current_time = time(NULL);
    printf("Thread %ld ends. (t=%ld)\n", id, current_time - start_time);
    
    // CRITICAL SECTION END - Release access
    // Increment semaphore (unlock)
    operation[0].sem_op = 1;
    semop(semid, operation, 1);
    printf("Memory: unlocked by thread %ld\n\n", id);
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }
    
    // Convert and verify thread count
    int const nt = atoi(argv[1]);
    if (nt <= 0)
        err("Number of threads must be positive");

    // Create array of thread IDs using variable length array
    pthread_t tid[nt];
    void *retval;

    // Initialize semaphore control structure
    union semun arg;
    
    // Generate unique key for semaphore
    // Using '/tmp' directory and 'b' as project identifier
    if ((sem_key = ftok("/tmp", 'b')) < 0)
        err("ftok");
    
    // Create semaphore set with one semaphore
    // 0644 = permission bits (rw-r--r--)
    // IPC_CREAT = create if not exists
    if ((semid = semget(sem_key, 1, 0644 | IPC_CREAT)) < 0)
        err("semget");
    
    // Set initial semaphore value to 2
    // This allows two threads to enter critical section simultaneously
    arg.val = 2;
    if ((semctl(semid, 0, SETVAL, arg)) < 0)
        err("semctl");
    
    // Configure semaphore operation structure
    operation[0].sem_num = 0;     // First (and only) semaphore in set
    operation[0].sem_flg = 0;     // Wait for resource (blocking mode)
    
    // Record program start time
    start_time = time(NULL);
    
    // Create threads
    for (long i = 0; i < nt; i++) {
        // Pass thread number as argument to thread_function
        if (pthread_create(&tid[i], NULL, thread_function, (void*)i) != 0)
            perror("pthread_create");
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < nt; i++) {
        if (pthread_join(tid[i], &retval) != 0)
            perror("pthread_join");
    }
    
    // Clean up: remove semaphore
    // IPC_RMID removes the semaphore set
    if (semctl(semid, 0, IPC_RMID) < 0)
        err("semctl cleanup");
    
    return 0;
}