#include <dirent.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>


int main (int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    time_t t_now = time(NULL);
    time_t t_prev = t_now - 100;

    assert(argc ==2);

    // Open
    if ((dp = opendir(argv[1])) == NULL) {
        perror("");
    }


    // Read
    while ((dirp = readdir(dp)) != NULL)
    {
        
        if (stat(dirp->d_name, &statbuf) < 0) {
            perror("");
        }
        if (statbuf.st_atime > t_prev) {
            printf("%s\n", dirp->d_name);
        }
        
    }

    // Close
    if (closedir(dp) < 0) {
        perror("");
    }

    return 0;
}