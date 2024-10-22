#include <dirent.h>
#include <stdio.h>
#include <assert.h>

int main (int argc, char *argv[])
{
    DIR *dp;
    struct dirent *dirp;

    assert(argc ==2);

    // Open
    if ((dp = opendir(argv[1])) == NULL)
        perror("");

    // Read
    while ((dirp = readdir(dp)) != NULL)
    {
        printf("%s\n", dirp->d_name);
    }

    // Close
    if (closedir(dp) < 0)
        perror("");

    return 0;
}