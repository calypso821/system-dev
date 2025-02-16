#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void print_usage(const char* program_name) {
    printf("Usage:\n");
    printf("  Read mode:  %s read [device]\n", program_name);
    printf("  Write mode: %s write [device] \"your text\"\n", program_name);
}

int main(int argc, char *argv[]) {
    int fd;
    char buffer[100];
    
    // Check arguments
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // Check mode (read/write)
    if (strcmp(argv[1], "read") == 0) {
        // Open for reading
        fd = open(argv[2], O_RDONLY);
        if (fd < 0) {
            perror("Error opening device for reading");
            return 1;
        }

        // Read from device
        int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("Error reading from device");
            close(fd);
            return 1;
        }

        // Null terminate and print
        buffer[bytes_read] = '\0';
        printf("Read from device: %s", buffer);

    } else if (strcmp(argv[1], "write") == 0) {
        // Check if we have text to write
        if (argc < 4) {
            printf("Error: No text provided for writing\n");
            print_usage(argv[0]);
            return 1;
        }

        // Open for writing
        fd = open(argv[2], O_WRONLY);
        if (fd < 0) {
            perror("Error opening device for writing");
            return 1;
        }

        // Write to device
        int bytes_written = write(fd, argv[3], strlen(argv[3]));
        if (bytes_written < 0) {
            perror("Error writing to device");
            close(fd);
            return 1;
        }
        printf("Wrote %d bytes to device\n", bytes_written);

    } else {
        printf("Invalid mode. Use 'read' or 'write'\n");
        print_usage(argv[0]);
        return 1;
    }

    // Close device
    close(fd);
    return 0;
}