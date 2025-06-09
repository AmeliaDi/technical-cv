/*
 * Test program for AmeliaDi Kernel Module IOCTL operations
 * 
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>

#define DEVICE_PATH "/dev/ameliadi_device"

/* IOCTL commands (must match kernel module) */
#define IOCTL_GET_STATS 0
#define IOCTL_CLEAR_BUFFER 1
#define IOCTL_RESET_STATS 2

/* Statistics structure (must match kernel module) */
struct module_stats {
    unsigned long open_count;
    unsigned long read_count;
    unsigned long write_count;
    unsigned long ioctl_count;
    unsigned long total_bytes_read;
    unsigned long total_bytes_written;
    long long start_time;  /* ktime_t simplified */
};

void print_stats(struct module_stats *stats) {
    printf("\n=== Device Statistics ===\n");
    printf("Open count: %lu\n", stats->open_count);
    printf("Read operations: %lu\n", stats->read_count);
    printf("Write operations: %lu\n", stats->write_count);
    printf("IOCTL operations: %lu\n", stats->ioctl_count);
    printf("Total bytes read: %lu\n", stats->total_bytes_read);
    printf("Total bytes written: %lu\n", stats->total_bytes_written);
    printf("=========================\n\n");
}

int main() {
    int fd;
    struct module_stats stats;
    char buffer[256];
    int choice;
    
    printf("AmeliaDi Kernel Module Test Program\n");
    printf("===================================\n\n");
    
    /* Open device */
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("Make sure the kernel module is loaded and you have root privileges\n");
        return 1;
    }
    
    printf("Device opened successfully!\n");
    
    while (1) {
        printf("\nChoose an operation:\n");
        printf("1. Write data to device\n");
        printf("2. Read data from device\n");
        printf("3. Get statistics (IOCTL)\n");
        printf("4. Clear buffer (IOCTL)\n");
        printf("5. Reset statistics (IOCTL)\n");
        printf("6. Performance test\n");
        printf("0. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n'); /* Clear input buffer */
            continue;
        }
        
        switch (choice) {
            case 1: /* Write data */
                printf("Enter data to write: ");
                getchar(); /* Consume newline */
                if (fgets(buffer, sizeof(buffer), stdin)) {
                    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
                    if (bytes_written >= 0) {
                        printf("Wrote %zd bytes successfully\n", bytes_written);
                    } else {
                        perror("Write failed");
                    }
                }
                break;
                
            case 2: /* Read data */
                memset(buffer, 0, sizeof(buffer));
                lseek(fd, 0, SEEK_SET); /* Reset file pointer */
                ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
                if (bytes_read >= 0) {
                    buffer[bytes_read] = '\0';
                    printf("Read %zd bytes: '%s'\n", bytes_read, buffer);
                } else {
                    perror("Read failed");
                }
                break;
                
            case 3: /* Get statistics */
                if (ioctl(fd, IOCTL_GET_STATS, &stats) == 0) {
                    print_stats(&stats);
                } else {
                    perror("IOCTL get stats failed");
                }
                break;
                
            case 4: /* Clear buffer */
                if (ioctl(fd, IOCTL_CLEAR_BUFFER, 0) == 0) {
                    printf("Buffer cleared successfully\n");
                } else {
                    perror("IOCTL clear buffer failed");
                }
                break;
                
            case 5: /* Reset statistics */
                if (ioctl(fd, IOCTL_RESET_STATS, 0) == 0) {
                    printf("Statistics reset successfully\n");
                } else {
                    perror("IOCTL reset stats failed");
                }
                break;
                
            case 6: /* Performance test */
                printf("Running performance test...\n");
                clock_t start_time = clock();
                
                /* Write test */
                for (int i = 0; i < 1000; i++) {
                    snprintf(buffer, sizeof(buffer), "Test data %d", i);
                    write(fd, buffer, strlen(buffer));
                }
                
                /* Read test */
                for (int i = 0; i < 1000; i++) {
                    lseek(fd, 0, SEEK_SET);
                    read(fd, buffer, sizeof(buffer));
                }
                
                clock_t end_time = clock();
                double elapsed = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
                printf("Performance test completed in %.2f seconds\n", elapsed);
                printf("Operations: 2000 (1000 writes + 1000 reads)\n");
                printf("Average: %.2f ops/sec\n", 2000.0 / elapsed);
                
                /* Show final statistics */
                if (ioctl(fd, IOCTL_GET_STATS, &stats) == 0) {
                    print_stats(&stats);
                }
                break;
                
            case 0: /* Exit */
                printf("Exiting...\n");
                close(fd);
                return 0;
                
            default:
                printf("Invalid choice!\n");
                break;
        }
    }
    
    close(fd);
    return 0;
} 