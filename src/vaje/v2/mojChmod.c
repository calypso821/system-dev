#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

void mode_to_str(mode_t mode, char *str);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: mode <filename>\n");
        return 0;
    }

    const char *path = argv[2];
    struct stat statbuf;
    mode_t mode;

    // Parse mode 
    if (atoi(argv[1]) || argv[1][0] == '0') {
        // Octal format
        char *ep;
        mode = strtol(argv[1], &ep, 8);
        if (*ep != '\0') {
            perror("Error parsing octal string mdoe");
            return 1;
        }
    } else {
        // Set current file mode
        stat(path, &statbuf);
        mode = statbuf.st_mode;

        // Set - User/Group-ID bit (S_ISUID, S_ISGUID)
        // 1. OR | - keep all bits both of mode (1) and S_ISUID (1)

        // Clear - User/Group-ID bit (S_ISUID, S_ISGUID)
        // 1. negate S_ISUID (set all bits to 1 except S_ISUID to 0)
        // 2. AND & - keep all bits (1) except S_ISUID (0)

        if (strcmp(argv[1], "u+s") == 0) {
            // Set - User-ID
            mode = mode | S_ISUID;
        } else if (strcmp(argv[1], "u-s") == 0) {
            // Clear - User-ID
            mode = mode & ~S_ISUID;   
        } else if (strcmp(argv[1], "g+s") == 0) {
            // Set Group-ID
            mode = mode | S_ISGID;
        } else if (strcmp(argv[1], "g-s") == 0) {
            // Clear - Group-ID
            mode = mode & ~S_ISGID;   
        } else {
            perror("Error parsing input mode");
            return 1;
        }
    }

    if (chmod(path, mode) == -1) {
        perror("Error setting chmode");
        return 1;
    }

    // Create output string (mode - rwxrwxrwx + \0)  
    char output_str[10];
    mode_to_str(mode, output_str);
    printf("Nova dovoljenja: %s\n", output_str);

    return 0;
}

void mode_to_str(mode_t mode, char *str) {
    const char *rwx = "rwx";
    
    for (int i = 0; i < 3; i++) {
        int offset = (2 - i) * 3;
        // user (2 - 0) * 3 = 6 
        // group (2 - 1) * 3 = 3
        // others (2 - 2) * 3 = 0
        for (int j = 0; j < 3; j++) {
            // Check if bit is present
            // offset + j = bit position
            // 1 << 6 = shift 0000001 on left 6 times = 1000000

            // 1 << 6 creates:   01000000
            // mode (0664):      11011010
            //                   --------
            // AND result:       01000000
            // (non-zero means permission granted) - add char at pos j
            int mask = 1 << (offset + (2 - j));
            int bit = mode & mask;

            // Check special cases for execute bit positions
            if (j == 2) { // Execute bit position
                if (i == 0 && (mode & S_ISUID)) { // User
                    str[i * 3 + j] = bit ? 's' : 'S';
                    continue;
                }
                if (i == 1 && (mode & S_ISGID)) { // Group
                    str[i * 3 + j] = bit ? 's' : 'S';
                    continue;
                }
                if (i == 2 && (mode & S_ISVTX)) { // Others (sticky bit)
                    str[i * 3 + j] = bit ? 't' : 'T';
                    continue;
                }
            }
            
            // Set output string
            // If bit is set -> char at j
            // If bit is not set -> '-'
            str[i * 3 + j] = bit ? rwx[j] : '-';
        }
    }
    str[9] = '\0';
}
