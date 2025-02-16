#include <stdio.h>
#include <string.h>

void readLine(FILE *fp, char *buffer) {
    // Read a line into the buffer
    fgets(buffer, 100, fp);
    printf("Inside function, buffer contains: %s", buffer);
}

int main() {
    FILE *fp;
    char buffer[100];
    
    // First, let's create a test file
    fp = fopen("test.txt", "w");
    fputs("First line\n", fp);
    fputs("Second line\n", fp);
    fputs("Third line\n", fp);
    fclose(fp);
    
    // Now let's read from it
    fp = fopen("test.txt", "r");
    
    // First read in main
    fgets(buffer, 100, fp);
    printf("Main first read, buffer contains: %s", buffer);
    
    // Call function to read next line
    readLine(fp, buffer);
    
    // Read again in main
    fgets(buffer, 100, fp);
    printf("Main second read, buffer contains: %s", buffer);
    
    fclose(fp);
    return 0;
}