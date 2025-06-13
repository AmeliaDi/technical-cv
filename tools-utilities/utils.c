/*
 * Network Scanner Utilities
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>

#include "network_scanner.h"

// Get current timestamp with microsecond precision
double get_timestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Set socket timeout
int set_socket_timeout(int sockfd, int timeout_ms) {
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        return -1;
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        return -1;
    }
    
    return 0;
}

// Get local IP address
int get_local_ip(char *ip) {
    struct ifaddrs *ifaddrs_ptr, *ifa;
    
    if (getifaddrs(&ifaddrs_ptr) == -1) {
        return -1;
    }
    
    for (ifa = ifaddrs_ptr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                struct sockaddr_in *addr_in = (struct sockaddr_in *)ifa->ifa_addr;
                strcpy(ip, inet_ntoa(addr_in->sin_addr));
                freeifaddrs(ifaddrs_ptr);
                return 0;
            }
        }
    }
    
    freeifaddrs(ifaddrs_ptr);
    return -1;
}

// Get default gateway
int get_default_gateway(char *gateway) {
    FILE *fp = fopen("/proc/net/route", "r");
    if (!fp) {
        return -1;
    }
    
    char line[256];
    char iface[16];
    unsigned long dest, gw;
    
    // Skip header
    fgets(line, sizeof(line), fp);
    
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%15s %lx %lx", iface, &dest, &gw) == 3) {
            if (dest == 0) { // Default route
                struct in_addr addr;
                addr.s_addr = gw;
                strcpy(gateway, inet_ntoa(addr));
                fclose(fp);
                return 0;
            }
        }
    }
    
    fclose(fp);
    return -1;
}

// Get network interfaces
int get_network_interfaces(interface_info_t **interfaces, int *count) {
    struct ifaddrs *ifaddrs_ptr, *ifa;
    *count = 0;
    
    if (getifaddrs(&ifaddrs_ptr) == -1) {
        return -1;
    }
    
    // Count interfaces
    for (ifa = ifaddrs_ptr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            (*count)++;
        }
    }
    
    *interfaces = malloc(*count * sizeof(interface_info_t));
    int idx = 0;
    
    for (ifa = ifaddrs_ptr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            interface_info_t *iface = &(*interfaces)[idx];
            
            strncpy(iface->name, ifa->ifa_name, sizeof(iface->name) - 1);
            
            struct sockaddr_in *addr_in = (struct sockaddr_in *)ifa->ifa_addr;
            strcpy(iface->ip, inet_ntoa(addr_in->sin_addr));
            
            if (ifa->ifa_netmask) {
                struct sockaddr_in *netmask_in = (struct sockaddr_in *)ifa->ifa_netmask;
                strcpy(iface->netmask, inet_ntoa(netmask_in->sin_addr));
            }
            
            if (ifa->ifa_broadaddr) {
                struct sockaddr_in *broadcast_in = (struct sockaddr_in *)ifa->ifa_broadaddr;
                strcpy(iface->broadcast, inet_ntoa(broadcast_in->sin_addr));
            }
            
            iface->is_up = (ifa->ifa_flags & IFF_UP) != 0;
            idx++;
        }
    }
    
    freeifaddrs(ifaddrs_ptr);
    return 0;
}

// Check if IP is in local network
int is_local_network(const char *ip) {
    struct in_addr addr;
    inet_pton(AF_INET, ip, &addr);
    uint32_t ip_int = ntohl(addr.s_addr);
    
    // Check RFC 1918 private networks
    // 10.0.0.0/8
    if ((ip_int & 0xFF000000) == 0x0A000000) return 1;
    
    // 172.16.0.0/12
    if ((ip_int & 0xFFF00000) == 0xAC100000) return 1;
    
    // 192.168.0.0/16
    if ((ip_int & 0xFFFF0000) == 0xC0A80000) return 1;
    
    // 127.0.0.0/8 (loopback)
    if ((ip_int & 0xFF000000) == 0x7F000000) return 1;
    
    return 0;
}

// Resolve hostname to IP
int resolve_hostname(const char *hostname, char *ip) {
    struct hostent *host_entry = gethostbyname(hostname);
    if (!host_entry) {
        return -1;
    }
    
    struct in_addr addr;
    addr.s_addr = *((unsigned long *)host_entry->h_addr_list[0]);
    strcpy(ip, inet_ntoa(addr));
    
    return 0;
}

// Check privileges for raw sockets
int check_privileges(void) {
    if (geteuid() != 0) {
        return -1; // Not root
    }
    return 0;
}

// Validate target IP/hostname
int validate_target(const char *target) {
    struct in_addr addr;
    
    // Try to parse as IP address
    if (inet_pton(AF_INET, target, &addr) == 1) {
        return 0; // Valid IP
    }
    
    // Try to resolve as hostname
    char ip[MAX_IP_LEN];
    if (resolve_hostname(target, ip) == 0) {
        return 0; // Valid hostname
    }
    
    return -1; // Invalid
}

// Validate port range
int validate_port_range(uint16_t start, uint16_t end) {
    if (start == 0 || end == 0 || start > end || end > 65535) {
        return -1;
    }
    return 0;
}

// Randomize source port
int randomize_source_port(void) {
    srand(time(NULL) ^ getpid());
    return 1024 + (rand() % (65535 - 1024));
}

// Randomize scan order
void randomize_scan_order(uint16_t *ports, int count) {
    srand(time(NULL));
    
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        uint16_t temp = ports[i];
        ports[i] = ports[j];
        ports[j] = temp;
    }
}

// Calculate RTT to target
double calculate_rtt(const char *target_ip) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80); // Try port 80
    inet_pton(AF_INET, target_ip, &addr.sin_addr);
    
    double start_time = get_timestamp();
    int result = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    double end_time = get_timestamp();
    
    close(sockfd);
    
    if (result == 0) {
        return (end_time - start_time) * 1000; // Return RTT in ms
    }
    
    return -1;
}

// Adaptive timing based on success rate
void adaptive_timing(double *delay, int success_rate) {
    if (success_rate > 95) {
        *delay *= 0.9; // Decrease delay if high success rate
    } else if (success_rate < 50) {
        *delay *= 1.5; // Increase delay if low success rate
    }
    
    // Clamp delay between 1ms and 1000ms
    if (*delay < 0.001) *delay = 0.001;
    if (*delay > 1.0) *delay = 1.0;
}

// Implement rate limiting
void implement_rate_limit(int packets_per_second) {
    static double last_packet_time = 0;
    double min_interval = 1.0 / packets_per_second;
    
    double current_time = get_timestamp();
    double elapsed = current_time - last_packet_time;
    
    if (elapsed < min_interval) {
        double sleep_time = min_interval - elapsed;
        usleep((useconds_t)(sleep_time * 1000000));
    }
    
    last_packet_time = get_timestamp();
}

// Handle scanner errors
void handle_scanner_error(const char *function, int error_code) {
    fprintf(stderr, "[ERROR] %s failed with code %d: %s\n", 
            function, error_code, strerror(error_code));
}

// Log scan activity
void log_scan_activity(const char *message) {
    FILE *log_file = fopen("scanner.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0'; // Remove newline
        
        fprintf(log_file, "[%s] %s\n", time_str, message);
        fclose(log_file);
    }
}

// Generate unique scan ID
void generate_scan_id(char *scan_id, size_t size) {
    time_t now = time(NULL);
    snprintf(scan_id, size, "scan_%ld_%d", now, getpid());
}

// Convert bytes to human readable format
void format_bytes(uint64_t bytes, char *output, size_t size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double value = bytes;
    
    while (value >= 1024 && unit_index < 4) {
        value /= 1024;
        unit_index++;
    }
    
    snprintf(output, size, "%.2f %s", value, units[unit_index]);
}

// Parse time duration string (e.g., "5m", "30s", "2h")
int parse_duration(const char *duration_str, int *seconds) {
    int value;
    char unit;
    
    if (sscanf(duration_str, "%d%c", &value, &unit) != 2) {
        return -1;
    }
    
    switch (unit) {
        case 's': *seconds = value; break;
        case 'm': *seconds = value * 60; break;
        case 'h': *seconds = value * 3600; break;
        case 'd': *seconds = value * 86400; break;
        default: return -1;
    }
    
    return 0;
}

// Generate random MAC address
void generate_random_mac(char *mac) {
    srand(time(NULL));
    snprintf(mac, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
             rand() % 256, rand() % 256, rand() % 256,
             rand() % 256, rand() % 256, rand() % 256);
}

// Check if process is running as root
int is_root(void) {
    return geteuid() == 0;
}

// Get process memory usage
size_t get_memory_usage(void) {
    FILE *status = fopen("/proc/self/status", "r");
    if (!status) return 0;
    
    char line[256];
    size_t memory = 0;
    
    while (fgets(line, sizeof(line), status)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %zu kB", &memory);
            memory *= 1024; // Convert to bytes
            break;
        }
    }
    
    fclose(status);
    return memory;
}

// Create directory if it doesn't exist
int create_directory(const char *path) {
    struct stat st = {0};
    
    if (stat(path, &st) == -1) {
        return mkdir(path, 0755);
    }
    
    return 0; // Directory already exists
}

// Safe string copy
void safe_strncpy(char *dest, const char *src, size_t size) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

// Trim whitespace from string
char* trim_whitespace(char *str) {
    char *end;
    
    // Trim leading space
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str; // All spaces
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
} 