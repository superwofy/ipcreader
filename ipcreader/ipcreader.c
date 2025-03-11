/**
 * QNX IPC Channel Reader
 *
 * A simple utility to read data from QNX IPC channels.
 * Usage: ipc_reader <channel_number|raw_rx|raw_tx> [j]
 * If 'j' is provided, reads from jacinto qnet path
 * Special cases:
 *   raw_rx - reads from /dev/ipc/raw/rx_0 or /net/hu-jacinto/dev/ipc/raw/rx_0
 *   raw_tx - reads from /dev/ipc/raw/tx_0 or /net/hu-jacinto/dev/ipc/raw/tx_0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096
#define MAX_PATH_LEN 128

int main(int argc, char *argv[]) {
    char path[MAX_PATH_LEN];
    int fd;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE];
    int channel_num = 0;
    int use_jacinto = 0;
    int use_raw_rx = 0;
    int use_raw_tx = 0;

    // Check command line arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <channel_number|raw_rx|raw_tx> [j]\n", argv[0]);
        fprintf(stderr, "Add 'j' parameter to read from jacinto system\n");
        return EXIT_FAILURE;
    }

    // Check if special raw_rx or raw_tx mode is specified
    if (strcmp(argv[1], "raw_rx") == 0) {
        use_raw_rx = 1;
    } else if (strcmp(argv[1], "raw_tx") == 0) {
        use_raw_tx = 1;
    } else {
        // Parse channel number for normal mode
        channel_num = atoi(argv[1]);
        if (channel_num <= 0) {
            fprintf(stderr, "Invalid channel number: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    // Check if jacinto mode is specified
    if (argc == 3 && (argv[2][0] == 'j' || argv[2][0] == 'J')) {
        use_jacinto = 1;
    }

    // Construct the path to the IPC channel
    if (use_raw_rx) {
        if (use_jacinto) {
            snprintf(path, MAX_PATH_LEN, "/net/hu-jacinto/dev/ipc/raw/rx_0");
        } else {
            snprintf(path, MAX_PATH_LEN, "/dev/ipc/raw/rx_0");
        }
    } else if (use_raw_tx) {
        if (use_jacinto) {
            snprintf(path, MAX_PATH_LEN, "/net/hu-jacinto/dev/ipc/raw/tx_0");
        } else {
            snprintf(path, MAX_PATH_LEN, "/dev/ipc/raw/tx_0");
        }
    } else {
        // Normal numbered IPC path
        if (use_jacinto) {
            snprintf(path, MAX_PATH_LEN, "/net/hu-jacinto/dev/ipc/ipc%d", channel_num);
        } else {
            snprintf(path, MAX_PATH_LEN, "/dev/ipc/ipc%d", channel_num);
        }
    }

    printf("Opening IPC channel: %s\n", path);

    // Open the IPC channel in non-blocking mode
    fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("Failed to open IPC channel");
        return EXIT_FAILURE;
    }

    printf("Successfully opened IPC channel %s%s\n",
           use_raw_rx ? "raw_rx" : (use_raw_tx ? "raw_tx" : argv[1]),
           use_jacinto ? " on jacinto system" : "");
    printf("Waiting for data (Press Ctrl+C to exit)...\n");

    // Read from the IPC channel
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Read data from the channel
        bytes_read = read(fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read > 0) {
            // Ensure null termination for string operations
            buffer[bytes_read] = '\0';

            // Print the data
            printf("\nReceived %zd bytes:\n", bytes_read);
            int i;

            // Print each byte in hex format
            for (i = 0; i < bytes_read; i++) {
                printf("%02X ", (unsigned char)buffer[i]);
            }

            printf("\n");
        }
        else if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available yet, sleep and try again
                usleep(1000); // Sleep for 1ms
            } else {
                // Real error occurred
                perror("Error reading from IPC channel.");
                close(fd);
                return EXIT_FAILURE;
            }
        }
    }

    // Close the file descriptor (this will never be reached due to the infinite loop,
    // but included for completeness)
    close(fd);

    return EXIT_SUCCESS;
}
