#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NT 8
#define P_MAX 400000000

// Global
// All points sum 
int ps = 0;
// All circle points sum
int zs = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *shoot_points(void *arg)
{
    // Create unique seed based of thread id
    unsigned int seed = (long)arg;
    // All points, points in circle
    int p = 10000000;
    int z;
    int itteration = 0;

    while (1)
    {
        z = 0;

        for (int i = 0; i < p; i++)
        {
            // rand_r() (and rand()) returns a random integer between 0 and RAND_MAX
            // rand() is not thread safe -? uses global state
            // Get random value between -1 and 1 (full circle)
            //double x = 2.0 * ((double)rand_r(&seed)) / RAND_MAX - 1.0;
            //double y = 2.0 * ((double)rand_r(&seed)) / RAND_MAX - 1.0;
            
            // 1 quadrant only (0 - 1) only
            double x = ((double)rand_r(&seed)) / RAND_MAX;
            double y = ((double)rand_r(&seed)) / RAND_MAX;

            // Point x,y -> randomly generated point in square (1 by 1)
            double dist = x*x + y*y;
            
            // Check if point is in circle
            if (dist <= 1)
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
            printf("Thread: %ld, Itteration: %d, New pi: %f\n",
                    (long)arg, itteration, 4.0 * zs / ps);
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

int main()
{
	pthread_t tid[NT];
	void *retval;
	
    // Create threads
	for (long i=0; i<NT; i++)	
		if (pthread_create(&tid[i], NULL, shoot_points, (void *)i) != 0)
			perror("pthread_create");
	
    
    // Join threads
	for (int i=0; i<NT; i++)
		if (pthread_join(tid[i], &retval) != 0)
			perror("pthread_join");

    // Final pi 
    printf("Final pi: %f\n", 4.0 * zs / ps);
    printf("PS: %d\n", ps);
	
	return 0;
}