#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define PATH_MAX 256

char get_file_type(struct stat *statbuf);
void print_mode(struct stat *statbuf);

int main(int argc, char* argv[]) {
    char *dir;
    if (argc == 2 && strcmp(argv[1], "-l") == 0) {
        // Current directory
        dir = ".";
    } else if (argc == 3 && strcmp(argv[1], "-l") == 0) {
        // Input directory
        dir = argv[2];
    } else {
        printf("Usage: %s -l\n", argv[0]);
        return 0;
    }

    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;
    char fullpath[PATH_MAX];


    // Open current dir
    if ((dp = opendir(dir)) == NULL) {
        perror("Error opening current dirrectory"); 
        return 1;
    }

    // Read
    while ((dirp = readdir(dp)) != NULL)
    {
        // Construcut full paht = dir and file name
        sprintf(fullpath, "%s/%s", dir, dirp->d_name);

        // Check type (dict, regular file...)
        if (stat(fullpath, &statbuf) < 0) {
            perror("");
        }
        // Print file type
        printf("%c", get_file_type(&statbuf));

        // Print mode
        print_mode(&statbuf);

        // Print hard links
        printf(" %lu", statbuf.st_nlink); 

        // name
        printf(" %s\n", dirp->d_name);
    }

    // Close
    if (closedir(dp) < 0) {
        perror("");
        return 1;
    }

    return 0;
}

char get_file_type(struct stat *statbuf) {
    // if (S_ISREG(statbuf->st_mode))
    // // It expands to:
    // if ((statbuf->st_mode & S_IFMT) == S_IFREG)
    // S_IFMT = File type MASK (mask out file tpye bits)
    // S_IFREG = Regular file

    if (S_ISREG(statbuf->st_mode))
        return '-'; // Regular file
    else if (S_ISDIR(statbuf->st_mode))
        return 'd'; // Directory
    else if (S_ISCHR(statbuf->st_mode))
        return 'c';
    else if (S_ISBLK(statbuf->st_mode))
        return 'b';
    else if (S_ISFIFO(statbuf->st_mode))
        return 'p';
    else if (S_ISLNK(statbuf->st_mode))
        return 'l';
    else if (S_ISSOCK(statbuf->st_mode))
        return 's';
    else
        return '?';
}

void print_mode(struct stat *statbuf) {
    // S_IRUSR = 0400 (octal)
    //     = 0b100 000 000 (binary)
    //     = 256 (decimal)

    // Owner permissions
    printf("%c", (statbuf->st_mode & S_IRUSR) ? 'r' : '-'); // read
    printf("%c", (statbuf->st_mode & S_IWUSR) ? 'w' : '-'); // write
    //printf("%c", (statbuf->st_mode & S_IXUSR) ? 'x' : '-'); // execute

    // & And opertation (check if bit 8 is active)
    if (statbuf->st_mode & S_ISUID)
        printf("%c", (statbuf->st_mode & S_IXUSR) ? 's' : 'S');
    else
        printf("%c", (statbuf->st_mode & S_IXUSR) ? 'x' : '-');

    // Group permissions
    printf("%c", (statbuf->st_mode & S_IRGRP) ? 'r' : '-'); // read
    printf("%c", (statbuf->st_mode & S_IWGRP) ? 'w' : '-'); // write
    //printf("%c", (statbuf->st_mode & S_IXGRP) ? 'x' : '-'); // execute

    // Execute
    if (statbuf->st_mode & S_ISGID)
        printf("%c", (statbuf->st_mode & S_IXGRP) ? 's' : 'S');
    else
        printf("%c", (statbuf->st_mode & S_IXGRP) ? 'x' : '-');

    // Others permissions
    printf("%c", (statbuf->st_mode & S_IROTH) ? 'r' : '-'); // read
    printf("%c", (statbuf->st_mode & S_IWOTH) ? 'w' : '-'); // write
    //printf("%c", (statbuf->st_mode & S_IXOTH) ? 'x' : '-'); // execute

    // Execute
    if (statbuf->st_mode & S_ISVTX)
        printf("%c", (statbuf->st_mode & S_IXOTH) ? 't' : 'T');
    else
        printf("%c", (statbuf->st_mode & S_IXOTH) ? 'x' : '-');
}

// Binary: 001 000 000 110 100 000

// Breaking it down from right to left (each 3 bits = 1 octal digit):
// 000 = 0
// 100 = 4
// 110 = 6
// 000 = 0
// 000 = 0
// 001 = 1

// So: binary 001 000 000 110 100 000 = octal 100640

// Meaning of these bits:
// 001 000 000  | 110 | 100 | 000
//    special   | usr | grp | oth
//    (1)       | (6) | (4) | (0)

// Which means:
// - Special bits: 1 (SUID set)
// - User permissions: 6 (rw-)
// - Group permissions: 4 (r--)
// - Others permissions: 0 (---)

// Special bits
// Position mapping:
// Owner:  s/S (SUID)    - Set User ID
// Group:  s/S (SGID)    - Set Group ID
// Others: t/T (Sticky)  - Sticky bit (for temp files)

// 's' (lowercase) = SUID is set AND execute is set
// - S_ISUID (4000) + S_IXUSR (100) = 4100 octal
// - User can execute AND program runs with owner's privileges

// 'S' (uppercase) = SUID is set but execute is NOT set
// - S_ISUID (4000) only, no S_IXUSR
// - Program has SUID but can't be executed (usually a misconfiguration)

// 't' (lowercase) = Sticky bit (S_ISVTX) AND execute bit are set
// - S_ISVTX (1000) + S_IXOTH (001) 
// - Directory can be executed (searched/accessed) and sticky bit is active
// - Common on directories like /tmp

// 'T' (uppercase) = Sticky bit set but execute is NOT set
// - Just S_ISVTX (1000), no execute
// - Rarely seen, usually a misconfiguration
