#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// -lm (link math)

#define P_MAX 1e9
#define PI 3.14159265358979323846

// Global
// All points sum 
int ps = 0;
// All circle points sum
int zs = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct timespec timeStart, timeEnd;

void *shoot_points(void *arg)
{
    // Create unique seed based of thread id
    // unsigned int seed = (unsigned int)arg;
    // Seed based of current time
    unsigned int seed = (unsigned int)time(NULL);
    // All points, points in circle
    int p = 1e6;
    int z;
    int itteration = 0;

    while (1)
    {
        z = 0;

        for (int i = 0; i < p; i++)
        {
            // rand_r() (and rand()) returns a random integer between 0 and RAND_MAX
            // rand() is not thread safe -? uses global state
            
            // x get value for x [0, PI]
            double x = ((double)rand_r(&seed)) / RAND_MAX * PI;
            // y get possible value for sin --> [0, 1] only 
            double y = ((double)rand_r(&seed)) / RAND_MAX;

            // Check if generated y is under sin(x) curve
            if (y < sin(x))
            {
                z++;
            }
        }

        pthread_mutex_lock(&mut);
        // Check if number of points exceeds MAX
        if (ps < P_MAX)
        {
            // Add to global sums
            ps += p;
            zs += z;
            // Intergal area sin(x) [0, PI]
            // Rectangle area [1, PI] = PI
            // Probability = sin(x) area / PI
            // sin(x) area = PI * P
            // printf("Thread: %ld, Itteration: %d, New sin(x) area: %f\n",
            //         (long)arg, itteration, PI * ((double)zs / ps));
            fflush(stdout);
        }
        else 
        {
            pthread_mutex_unlock(&mut);
            break;
        }
        // Unlock for next itteration
        pthread_mutex_unlock(&mut);

        itteration++;
    }
   

	return(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <Nthreads>\n", argv[0]);
        return 0;
    }

    int const nt = atoi(argv[1]);
	pthread_t tid[nt];
	void *retval;

    // Start measuring time
    clock_gettime(CLOCK_REALTIME, &timeStart);
	
    // Create threads
	for (long i=0; i<nt; i++)	
		if (pthread_create(&tid[i], NULL, shoot_points, (void *)i) != 0)
			perror("pthread_create");
	
    
    // Join threads
	for (int i=0; i<nt; i++)
		if (pthread_join(tid[i], &retval) != 0)
			perror("pthread_join");

    // Stop measuring time
    clock_gettime(CLOCK_REALTIME, &timeEnd);

    double elapsed_time = (timeEnd.tv_sec - timeStart.tv_sec) + // seconds (left side of fp .)
                          (timeEnd.tv_nsec - timeStart.tv_nsec) * 1e-9; // nano seconds (right side of fp .)
    printf("Elapsed time: %.9f seconds\n", elapsed_time);

    // Final pi S
    printf("Final sin(x) [0, PI] area: %f\n", PI * ((double)zs / ps));
    printf("PS: %d\n", ps);
	
	return 0;
}