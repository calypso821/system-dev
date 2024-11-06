#include <stdio.h>
#include <string.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFSIZE 254

void copy_all_lines(FILE *src, FILE *dst, char *buf, size_t buf_size);
void copy_n_lines(FILE *src, FILE *dst, char *buf, size_t buf_size, int n); 

int main (int argc, char *argv[])
{
    char *mode;
    int n;

    if (argc == 3 && strcmp(argv[2], "-a") == 0) {
        // Append mode
        mode = "a";
    }
    else if (argc == 4 && strcmp(argv[2], "-d") == 0) {
        // Delete mode
        n = atoi(argv[3]) - 1;
        mode = "r+";
    }
    else if (argc == 4 && strcmp(argv[2], "-i") == 0) {
        // Insert mode
        n = atoi(argv[3]) - 1;
        mode = "r+";
    }
    else {
        fprintf(stderr, "Invalid arguments\n");
        fprintf(stderr, "Usage: %s <filename> [-a | -d <n> | -i <n>]\n", argv[0]);
        return 1;
    }

    FILE *fp;
    char buf[BUFFSIZE];
    char *line;

    if ((fp = fopen(argv[1], mode)) == NULL) {
        err(1, "failed opening file %s", argv[1]);
    }

    
    if (strcmp(argv[2], "-a") == 0)
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
    else if (strcmp(argv[2], "-d") == 0)
    {
        // Delete mode
        FILE *temp;
        if ((temp = fopen("temp.txt", "w")) == NULL) {
            err(1, "failed opening temp file");
        }

        // Write n lines fron original to temp file
        copy_n_lines(fp, temp, buf, sizeof(buf), n);
        
        // Read 1 line -> will be skipped (deleted)
        fgets(buf, sizeof(buf), fp);

        // Write rest of lines from original to temp file
        copy_all_lines(fp, temp, buf, sizeof(buf));

        fclose(temp);
        fclose(fp);
        // Delete original
        unlink(argv[1]);      
        // Rename temp              
        rename("temp.txt", argv[1]);  
    }
    else if (strcmp(argv[2], "-i") == 0)
    {
        // Insert mode
        FILE *temp;
        if ((temp = fopen("temp.txt", "w")) == NULL) {
            err(1, "failed opening temp file");
        }

        // Write n lines fron original to temp file
        copy_n_lines(fp, temp, buf, sizeof(buf), n);
        
        // Insert new line
        fputc('\n', temp);

        // Write rest of lines from original to temp file
        copy_all_lines(fp, temp, buf, sizeof(buf));

        fclose(temp);
        fclose(fp);
        unlink(argv[1]);                    // Delete original
        rename("temp.txt", argv[1]);  // Rename temp
    }
    return 0;
}

void copy_all_lines(FILE *src, FILE *dst, char *buf, size_t buf_size) 
{
    while (fgets(buf, buf_size, src) != NULL) {
        if (fputs(buf, dst) == EOF) {
            err(1, "fputs failed");
        }
    }
    if (ferror(src)) {
        err(1, "fgets failed");
    }
}

void copy_n_lines(FILE *src, FILE *dst, char *buf, size_t buf_size, int n) 
{
    for (int i = 0; i < n && fgets(buf, buf_size, src) != NULL; i++) {
        if (fputs(buf, dst) == EOF) {
            err(1, "fputs failed");
        }
    }
    if (ferror(src)) {
        err(1, "fgets failed");
    }
}