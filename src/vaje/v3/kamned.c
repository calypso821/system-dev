#include <stdio.h>

#define BUFSIZE 254

int main (int argc, char *argv)
{
    char *mode;
    int n;

    if (argc == 3 && strcmp(argv[2], "-a") == 0) {
        // Append mode
        mode = "a";
    }
    else if (argc == 4 && strcmp(argv[2], "-d") == 0) {
        // Delete mode
        n = atoi(argv[3]);
        mode = "r+";
    }
    else if (argc == 4 && strcmp(argv[2], "-i") == 0) {
        // Insert mode
        n = atoi(argv[3]);
        mode = "r+";
    }
    else {
        fprintf(stderr, "Invalid arguments\n");
        fprintf(stderr, "Usage: %s <filename> [-a | -d <n> | -i <n>]\n", argv[0]);
        return 1;
    }

    FILE *fp;
    char buf[BUFSIZE];
    char *line;

    if ((fp = fopen(argv[1], mode)) == NULL) {
        err(1, "failed opening file %s", argv[1]);
    }

    
    if (strcmp(argv[2], "-a"))
    {
        // Append mode

        // Read from stdin -> append to end of file
        // Stop Ctrl + D (EOF)
        // Line buffereed (stream propery) of stdin
        while (fgets(buf, sizeof(buf), stdin) != NULL)
        {
            // Full buffereed (stream propery) of fp
            if (fputs(buf, fp) == EOF) {
                err(1, "fputs filed");
            }
        }
        // Check exit status of wile loop (fgets)
        if (ferror(stdin)) {
            err(1, "fgets failed");
        }
        // If Ctrl + D pressed = EOF (no error)
    }
    else if (strcmp(argv[2], "-d"))
    {
        // Delete mode
        FILE *temp;
        if ((temp = fopen("temp.txt", "w")) == NULL) {
            err(1, "failed opening temp file");
        }
        int cnt = 0;

        // 1. Write all lines from original file to temp (execpt n line)
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            if (cnt != n) {
                if (fputs(buf, temp) == EOF) {
                    err(1, "fputs filed");
                }
            }
            cnt++;
        }
        // Check exit status of wile loop (fgets)
        if (ferror(fp)) {
            err(1, "fgets failed");
        }

        // 2. Write back all lines from temp to original file 
        // Set seek of temp to 0 
        // truncate fp
        // DOOOOO temp for bufferd
        
        while (fgets(buf, sizeof(buf), temp) != NULL)
        {
            if (fputs(buf, fp) == EOF) {
                err(1, "fputs filed");
            }
        }
        // Check exit status of wile loop (fgets)
        if (ferror(temp)) {
            err(1, "fgets failed");
        }

    }
}