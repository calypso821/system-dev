#include <stdio.h>



#define BUFSIZE1 10
#define BUFSIZE2 100

int main(int argc, char *argv)
{
    char *buf1[BUFSIZE1];
    char *buf2[BUFSIZE2];

    FILE *fp1;
    FILE *fp2;

    // Create both files (streams with file pointer)
    if ((fp1 = fopen("out1.txt", "w")) == NULL) {
        err("failed to open out1.txt");
    }
    if ((fp2 = fopen("out2.txt", "w")) == NULL) {
        err("failed to open out2.txt");
    }

    // Set buffers to full buffering mode
    if (setvbuf(fp1, buf1, _IOFBF, BUFSIZE1)) {
        err("failed to set bufffer 1");
    }
    if (setvbuf(fp2, buf2, _IOFBF, BUFSIZE2)) {
        err("failed to set buffer 2");
    }

    char *line = "Danes je lep dan!";

    // Izpisi 1000000 v out1 in out2
    time(NULL)




}