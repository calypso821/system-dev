#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

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
    if (argv[1][0] == "0") {
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
            int bit = mode & (1 << (offset + j));
            
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
// Others: t/T (Sticky)  - Sticky bit

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
