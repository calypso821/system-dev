#include <stdio.h>
#include <err.h>
#include <time.h>


#define BUFSIZE1 10
#define BUFSIZE2 100

int main(int argc, char *argv)
{
    // time_t start, end; // Second percision only
    struct timespec start, end;
    double elapsed;

    char buf[BUFSIZE2];

    FILE *fp1;
    FILE *fp2;

    // Create both files (streams with file pointer)
    if ((fp1 = fopen("build/out1.txt", "w")) == NULL) {
        err(1, "failed to open out1.txt");
    }
    if ((fp2 = fopen("build/out2.txt", "w")) == NULL) {
        err(1, "failed to open out2.txt");
    }

    char *line = "Danes je lep dan!\n"; // 19 chars (2x calls)
    //char *line = "Danes je\n"; // 10 chars (1x call)

    // Set buffer to full buffering mode and size of 10 and 100
    if (setvbuf(fp1, buf, _IOFBF, BUFSIZE1)) {
        err(1, "failed to set bufffer");
    }
    if (setvbuf(fp2, buf, _IOFBF, BUFSIZE2)) {
        err(1, "failed to set buffer");
    }

    // Get current time (NULL)
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < 5000000; i++)
    {
        if (fputs(line, fp1) == EOF) {
            err(1, "fputs filed");
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    printf("Elapsed time fully buffered stream (buffer size: %d): %fs\n", BUFSIZE1, elapsed);


    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < 5000000; i++)
    {
        if (fputs(line, fp2) == EOF) {
            err(1, "fputs filed");
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    printf("Elapsed time fully buffered stream (buffer size: %d): %fs\n", BUFSIZE2, elapsed);

    // line length = 20
    // Buffer 1 (size 10) - 2x system calls per fputs()
    // 5x fputs() ... 10x syscalls
    // Buffer 2 (size 100) - 1x system call per 5 fputs()
    // 5x fputs() ... 1x syscall



    if (fclose(fp1) == EOF) {
        err(1, "faild closing stream");
    }
    if (fclose(fp2) == EOF) {
        err(1, "faild closing stream");
    }

    return 0;
}