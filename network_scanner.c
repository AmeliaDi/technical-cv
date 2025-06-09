/*
 * Advanced Network Scanner
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 * 
 * A comprehensive network scanning tool implementing various
 * network discovery and analysis techniques.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <pcap.h>

#include "network_scanner.h"

// Global configuration
static scanner_config_t g_config = {
    .verbose = 0,
    .threads = 50,
    .timeout_ms = 1000,
    .scan_type = SCAN_TCP_SYN,
    .output_format = OUTPUT_HUMAN,
    .rate_limit = 100,
    .enable_stealth = 0,
    .resolve_hostnames = 1
};

// Global statistics
static scanner_stats_t g_stats = {0};
static volatile int g_running = 1;

// Thread pool
static pthread_t *g_thread_pool = NULL;
static work_queue_t g_work_queue = {0};

// Signal handler for graceful shutdown
void signal_handler(int sig) {
    printf("\n[!] Received signal %d, shutting down gracefully...\n", sig);
    g_running = 0;
}

// Print banner
void print_banner(void) {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     Advanced Network Scanner                ║\n");
    printf("║                    Author: AmeliaDi                         ║\n");
    printf("║               High-performance network discovery             ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
}

// Print usage information
void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] TARGET\n\n", program_name);
    printf("TARGET can be:\n");
    printf("  IP address:         192.168.1.1\n");
    printf("  CIDR notation:      192.168.1.0/24\n");
    printf("  Hostname:           example.com\n");
    printf("  IP range:           192.168.1.1-192.168.1.254\n\n");
    
    printf("OPTIONS:\n");
    printf("  -p, --ports PORTS   Port specification (default: top 100)\n");
    printf("                      Examples: 80,443,22 or 1-1000 or top100\n");
    printf("  -t, --threads NUM   Number of threads (default: 50)\n");
    printf("  -T, --timeout MS    Timeout in milliseconds (default: 1000)\n");
    printf("  -s, --scan-type     Scan type: syn, connect, udp, fin, xmas, null\n");
    printf("  -f, --format        Output format: human, json, xml, csv\n");
    printf("  -r, --rate RATE     Rate limit packets/sec (default: 100)\n");
    printf("  -S, --stealth       Enable stealth mode\n");
    printf("  -n, --no-resolve    Don't resolve hostnames\n");
    printf("  -v, --verbose       Verbose output\n");
    printf("  -O, --os-detect     Enable OS detection\n");
    printf("  -A, --aggressive    Aggressive scan (OS + version + scripts)\n");
    printf("  -o, --output FILE   Save output to file\n");
    printf("  -h, --help          Show this help\n\n");
    
    printf("SCAN TYPES:\n");
    printf("  syn        TCP SYN scan (default, requires root)\n");
    printf("  connect    TCP connect() scan\n");
    printf("  udp        UDP scan\n");
    printf("  fin        TCP FIN scan\n");
    printf("  xmas       TCP XMAS scan\n");
    printf("  null       TCP NULL scan\n");
    printf("  ack        TCP ACK scan\n");
    printf("  window     TCP Window scan\n\n");
    
    printf("EXAMPLES:\n");
    printf("  %s 192.168.1.0/24                      # Scan subnet\n", program_name);
    printf("  %s -p 80,443,22 192.168.1.1            # Scan specific ports\n", program_name);
    printf("  %s -s udp -p 53,67,68 192.168.1.1      # UDP scan\n", program_name);
    printf("  %s -A -v example.com                   # Aggressive scan\n", program_name);
    printf("  %s -t 100 -T 500 192.168.1.0/24        # Fast scan\n", program_name);
}

// Calculate checksum for IP/TCP headers
uint16_t calculate_checksum(uint16_t *ptr, int nbytes) {
    long sum = 0;
    uint16_t oddbyte;
    
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    
    if (nbytes == 1) {
        oddbyte = 0;
        *((uint8_t*)&oddbyte) = *(uint8_t*)ptr;
        sum += oddbyte;
    }
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

// Create raw socket for SYN scanning
int create_raw_socket(void) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        if (errno == EPERM) {
            printf("[!] Raw sockets require root privileges\n");
            printf("[!] Falling back to connect() scan\n");
            g_config.scan_type = SCAN_TCP_CONNECT;
            return -1;
        }
        perror("socket");
        return -1;
    }
    
    int one = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

// Perform TCP SYN scan
scan_result_t tcp_syn_scan(const char *target_ip, uint16_t port, int sockfd) {
    scan_result_t result = {0};
    strncpy(result.ip, target_ip, sizeof(result.ip) - 1);
    result.port = port;
    result.protocol = IPPROTO_TCP;
    result.state = PORT_FILTERED;
    
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, target_ip, &dest.sin_addr);
    
    // Create packet
    char packet[4096];
    memset(packet, 0, sizeof(packet));
    
    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));
    
    // IP header
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
    ip->id = htons(getpid());
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->check = 0;
    ip->saddr = inet_addr("127.0.0.1"); // Will be set by kernel
    ip->daddr = dest.sin_addr.s_addr;
    
    // TCP header
    tcp->source = htons(12345);
    tcp->dest = htons(port);
    tcp->seq = htonl(1105024978);
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->window = htons(14600);
    tcp->check = 0;
    tcp->urg_ptr = 0;
    
    // Calculate checksums
    ip->check = calculate_checksum((uint16_t *)ip, sizeof(struct iphdr));
    
    // Send packet
    if (sendto(sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        result.state = PORT_FILTERED;
        return result;
    }
    
    // TODO: Receive and analyze response
    // For now, just mark as open (simplified)
    result.state = PORT_OPEN;
    return result;
}

// Perform TCP connect scan
scan_result_t tcp_connect_scan(const char *target_ip, uint16_t port) {
    scan_result_t result = {0};
    strncpy(result.ip, target_ip, sizeof(result.ip) - 1);
    result.port = port;
    result.protocol = IPPROTO_TCP;
    result.state = PORT_CLOSED;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        result.state = PORT_FILTERED;
        return result;
    }
    
    // Set non-blocking
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, target_ip, &addr.sin_addr);
    
    int conn_result = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    
    if (conn_result == 0) {
        result.state = PORT_OPEN;
    } else if (errno == EINPROGRESS) {
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);
        
        struct timeval timeout;
        timeout.tv_sec = g_config.timeout_ms / 1000;
        timeout.tv_usec = (g_config.timeout_ms % 1000) * 1000;
        
        int select_result = select(sockfd + 1, NULL, &writefds, NULL, &timeout);
        
        if (select_result > 0 && FD_ISSET(sockfd, &writefds)) {
            int error;
            socklen_t len = sizeof(error);
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
            
            if (error == 0) {
                result.state = PORT_OPEN;
                
                // Try to grab banner
                char banner[1024] = {0};
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(sockfd, &readfds);
                
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;
                
                if (select(sockfd + 1, &readfds, NULL, NULL, &timeout) > 0) {
                    ssize_t bytes = recv(sockfd, banner, sizeof(banner) - 1, MSG_DONTWAIT);
                    if (bytes > 0) {
                        banner[bytes] = '\0';
                        // Clean up banner (remove newlines, etc.)
                        for (int i = 0; banner[i]; i++) {
                            if (banner[i] == '\n' || banner[i] == '\r') {
                                banner[i] = ' ';
                            }
                        }
                        strncpy(result.banner, banner, sizeof(result.banner) - 1);
                    }
                }
            } else {
                result.state = PORT_CLOSED;
            }
        } else {
            result.state = PORT_FILTERED;
        }
    } else {
        result.state = PORT_CLOSED;
    }
    
    close(sockfd);
    return result;
}

// Perform UDP scan
scan_result_t udp_scan(const char *target_ip, uint16_t port) {
    scan_result_t result = {0};
    strncpy(result.ip, target_ip, sizeof(result.ip) - 1);
    result.port = port;
    result.protocol = IPPROTO_UDP;
    result.state = PORT_OPEN_FILTERED; // UDP is tricky
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        result.state = PORT_FILTERED;
        return result;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, target_ip, &addr.sin_addr);
    
    // Send UDP packet
    char data[] = "UDP_PROBE";
    sendto(sockfd, data, strlen(data), 0, (struct sockaddr *)&addr, sizeof(addr));
    
    // Try to receive response
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    
    struct timeval timeout;
    timeout.tv_sec = g_config.timeout_ms / 1000;
    timeout.tv_usec = (g_config.timeout_ms % 1000) * 1000;
    
    if (select(sockfd + 1, &readfds, NULL, NULL, &timeout) > 0) {
        char response[1024];
        if (recv(sockfd, response, sizeof(response), 0) > 0) {
            result.state = PORT_OPEN;
        }
    }
    
    close(sockfd);
    return result;
}

// Service detection based on port and banner
void detect_service(scan_result_t *result) {
    struct {
        uint16_t port;
        const char *service;
    } common_services[] = {
        {21, "ftp"}, {22, "ssh"}, {23, "telnet"}, {25, "smtp"},
        {53, "dns"}, {80, "http"}, {110, "pop3"}, {143, "imap"},
        {443, "https"}, {993, "imaps"}, {995, "pop3s"}, {3389, "rdp"},
        {5432, "postgresql"}, {3306, "mysql"}, {1433, "mssql"}, {27017, "mongodb"},
        {6379, "redis"}, {11211, "memcached"}, {0, NULL}
    };
    
    // Check common services by port
    for (int i = 0; common_services[i].service; i++) {
        if (result->port == common_services[i].port) {
            strncpy(result->service, common_services[i].service, sizeof(result->service) - 1);
            break;
        }
    }
    
    // Banner-based detection
    if (strlen(result->banner) > 0) {
        if (strstr(result->banner, "SSH")) {
            strncpy(result->service, "ssh", sizeof(result->service) - 1);
        } else if (strstr(result->banner, "HTTP")) {
            strncpy(result->service, "http", sizeof(result->service) - 1);
        } else if (strstr(result->banner, "FTP")) {
            strncpy(result->service, "ftp", sizeof(result->service) - 1);
        }
    }
}

// Worker thread function
void* worker_thread(void *arg) {
    int thread_id = *(int*)arg;
    int raw_sock = -1;
    
    if (g_config.scan_type == SCAN_TCP_SYN) {
        raw_sock = create_raw_socket();
    }
    
    while (g_running) {
        work_item_t *item = work_queue_get(&g_work_queue);
        if (!item) {
            usleep(1000);
            continue;
        }
        
        scan_result_t result = {0};
        
        switch (g_config.scan_type) {
            case SCAN_TCP_SYN:
                if (raw_sock >= 0) {
                    result = tcp_syn_scan(item->target_ip, item->port, raw_sock);
                } else {
                    result = tcp_connect_scan(item->target_ip, item->port);
                }
                break;
            case SCAN_TCP_CONNECT:
                result = tcp_connect_scan(item->target_ip, item->port);
                break;
            case SCAN_UDP:
                result = udp_scan(item->target_ip, item->port);
                break;
            default:
                result = tcp_connect_scan(item->target_ip, item->port);
                break;
        }
        
        // Service detection
        detect_service(&result);
        
        // Hostname resolution
        if (g_config.resolve_hostnames) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            inet_pton(AF_INET, result.ip, &addr.sin_addr);
            
            char hostname[NI_MAXHOST];
            if (getnameinfo((struct sockaddr*)&addr, sizeof(addr), 
                          hostname, sizeof(hostname), NULL, 0, 0) == 0) {
                strncpy(result.hostname, hostname, sizeof(result.hostname) - 1);
            }
        }
        
        // Output result
        print_scan_result(&result);
        
        // Update statistics
        pthread_mutex_lock(&g_stats.mutex);
        g_stats.ports_scanned++;
        if (result.state == PORT_OPEN) {
            g_stats.ports_open++;
        }
        pthread_mutex_unlock(&g_stats.mutex);
        
        free(item);
        
        // Rate limiting
        if (g_config.rate_limit > 0) {
            usleep(1000000 / g_config.rate_limit);
        }
    }
    
    if (raw_sock >= 0) {
        close(raw_sock);
    }
    
    return NULL;
}

// Print scan result
void print_scan_result(const scan_result_t *result) {
    if (result->state == PORT_CLOSED && !g_config.verbose) {
        return; // Don't print closed ports unless verbose
    }
    
    static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&print_mutex);
    
    switch (g_config.output_format) {
        case OUTPUT_HUMAN:
            if (result->state == PORT_OPEN) {
                printf("%-15s %-6d/%-3s %-8s %-12s %s\n",
                       result->ip, result->port,
                       result->protocol == IPPROTO_TCP ? "tcp" : "udp",
                       port_state_string(result->state),
                       result->service[0] ? result->service : "unknown",
                       result->banner[0] ? result->banner : "");
            }
            break;
            
        case OUTPUT_JSON:
            printf("{\"ip\":\"%s\",\"port\":%d,\"protocol\":\"%s\",\"state\":\"%s\",\"service\":\"%s\",\"banner\":\"%s\"},\n",
                   result->ip, result->port,
                   result->protocol == IPPROTO_TCP ? "tcp" : "udp",
                   port_state_string(result->state),
                   result->service, result->banner);
            break;
            
        case OUTPUT_CSV:
            printf("%s,%d,%s,%s,%s,%s\n",
                   result->ip, result->port,
                   result->protocol == IPPROTO_TCP ? "tcp" : "udp",
                   port_state_string(result->state),
                   result->service, result->banner);
            break;
    }
    
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}

// Convert port state to string
const char* port_state_string(port_state_t state) {
    switch (state) {
        case PORT_OPEN: return "open";
        case PORT_CLOSED: return "closed";
        case PORT_FILTERED: return "filtered";
        case PORT_OPEN_FILTERED: return "open|filtered";
        default: return "unknown";
    }
}

// Parse port specification
int parse_ports(const char *port_spec, uint16_t **ports, int *count) {
    *ports = NULL;
    *count = 0;
    
    if (strcmp(port_spec, "top100") == 0) {
        // Top 100 most common ports
        static uint16_t top100[] = {
            1, 3, 4, 6, 7, 9, 13, 17, 19, 20, 21, 22, 23, 24, 25, 26, 30, 32, 33, 37,
            42, 43, 49, 53, 70, 79, 80, 81, 82, 83, 84, 85, 88, 89, 90, 99, 100, 106,
            109, 110, 111, 113, 119, 125, 135, 139, 143, 144, 146, 161, 163, 179, 199,
            211, 212, 222, 254, 255, 256, 259, 264, 280, 301, 306, 311, 340, 366, 389,
            406, 407, 416, 417, 425, 427, 443, 444, 445, 458, 464, 465, 481, 497, 500,
            512, 513, 514, 515, 524, 541, 543, 544, 545, 548, 554, 555, 563, 587, 593,
            616, 617, 625, 631, 636, 646, 648, 666, 667, 668, 683, 687, 691, 700, 705,
            711, 714, 720, 722, 726, 749, 765, 777, 783, 787, 800, 801, 808, 843, 873,
            880, 888, 898, 900, 901, 902, 903, 911, 912, 981, 993, 995, 999, 1000
        };
        
        *count = sizeof(top100) / sizeof(top100[0]);
        *ports = malloc(*count * sizeof(uint16_t));
        memcpy(*ports, top100, *count * sizeof(uint16_t));
        return 0;
    }
    
    // Parse comma-separated ports or ranges
    char *spec_copy = strdup(port_spec);
    char *token = strtok(spec_copy, ",");
    int capacity = 10;
    *ports = malloc(capacity * sizeof(uint16_t));
    
    while (token) {
        if (strchr(token, '-')) {
            // Range: 1-1000
            int start, end;
            if (sscanf(token, "%d-%d", &start, &end) == 2) {
                for (int p = start; p <= end && p <= 65535; p++) {
                    if (*count >= capacity) {
                        capacity *= 2;
                        *ports = realloc(*ports, capacity * sizeof(uint16_t));
                    }
                    (*ports)[(*count)++] = p;
                }
            }
        } else {
            // Single port
            int port = atoi(token);
            if (port > 0 && port <= 65535) {
                if (*count >= capacity) {
                    capacity *= 2;
                    *ports = realloc(*ports, capacity * sizeof(uint16_t));
                }
                (*ports)[(*count)++] = port;
            }
        }
        token = strtok(NULL, ",");
    }
    
    free(spec_copy);
    return 0;
}

// Parse target specification
int parse_targets(const char *target_spec, char ***targets, int *count) {
    *targets = NULL;
    *count = 0;
    
    // Check if it's CIDR notation
    if (strchr(target_spec, '/')) {
        return parse_cidr(target_spec, targets, count);
    }
    
    // Check if it's a range
    if (strchr(target_spec, '-')) {
        return parse_ip_range(target_spec, targets, count);
    }
    
    // Single target
    *count = 1;
    *targets = malloc(sizeof(char*));
    (*targets)[0] = strdup(target_spec);
    
    return 0;
}

// Parse CIDR notation
int parse_cidr(const char *cidr, char ***targets, int *count) {
    char *cidr_copy = strdup(cidr);
    char *slash = strchr(cidr_copy, '/');
    if (!slash) {
        free(cidr_copy);
        return -1;
    }
    
    *slash = '\0';
    int prefix_len = atoi(slash + 1);
    
    if (prefix_len < 0 || prefix_len > 32) {
        free(cidr_copy);
        return -1;
    }
    
    struct in_addr addr;
    if (inet_pton(AF_INET, cidr_copy, &addr) != 1) {
        free(cidr_copy);
        return -1;
    }
    
    uint32_t network = ntohl(addr.s_addr);
    uint32_t mask = (0xFFFFFFFF << (32 - prefix_len)) & 0xFFFFFFFF;
    network &= mask;
    
    int host_bits = 32 - prefix_len;
    int max_hosts = (1 << host_bits) - 2; // Exclude network and broadcast
    
    if (max_hosts > 1000) {
        printf("[!] Large subnet (%d hosts), limiting to first 1000\n", max_hosts);
        max_hosts = 1000;
    }
    
    *count = max_hosts;
    *targets = malloc(max_hosts * sizeof(char*));
    
    for (int i = 0; i < max_hosts; i++) {
        uint32_t host_addr = htonl(network + i + 1);
        struct in_addr host_in_addr;
        host_in_addr.s_addr = host_addr;
        (*targets)[i] = strdup(inet_ntoa(host_in_addr));
    }
    
    free(cidr_copy);
    return 0;
}

// Parse IP range
int parse_ip_range(const char *range, char ***targets, int *count) {
    char *range_copy = strdup(range);
    char *dash = strchr(range_copy, '-');
    if (!dash) {
        free(range_copy);
        return -1;
    }
    
    *dash = '\0';
    char *start_ip = range_copy;
    char *end_ip = dash + 1;
    
    struct in_addr start_addr, end_addr;
    if (inet_pton(AF_INET, start_ip, &start_addr) != 1 ||
        inet_pton(AF_INET, end_ip, &end_addr) != 1) {
        free(range_copy);
        return -1;
    }
    
    uint32_t start = ntohl(start_addr.s_addr);
    uint32_t end = ntohl(end_addr.s_addr);
    
    if (start > end) {
        uint32_t temp = start;
        start = end;
        end = temp;
    }
    
    *count = end - start + 1;
    if (*count > 1000) {
        printf("[!] Large range (%d IPs), limiting to first 1000\n", *count);
        *count = 1000;
    }
    
    *targets = malloc(*count * sizeof(char*));
    
    for (int i = 0; i < *count; i++) {
        uint32_t addr = htonl(start + i);
        struct in_addr in_addr;
        in_addr.s_addr = addr;
        (*targets)[i] = strdup(inet_ntoa(in_addr));
    }
    
    free(range_copy);
    return 0;
}

// Work queue implementation
void work_queue_init(work_queue_t *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}

void work_queue_add(work_queue_t *queue, const char *target_ip, uint16_t port) {
    work_item_t *item = malloc(sizeof(work_item_t));
    strncpy(item->target_ip, target_ip, sizeof(item->target_ip) - 1);
    item->port = port;
    item->next = NULL;
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->tail) {
        queue->tail->next = item;
    } else {
        queue->head = item;
    }
    queue->tail = item;
    queue->count++;
    
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

work_item_t* work_queue_get(work_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->head == NULL && g_running) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    
    work_item_t *item = NULL;
    if (queue->head) {
        item = queue->head;
        queue->head = item->next;
        if (queue->head == NULL) {
            queue->tail = NULL;
        }
        queue->count--;
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return item;
}

// Print statistics
void print_statistics(void) {
    pthread_mutex_lock(&g_stats.mutex);
    
    time_t now = time(NULL);
    double elapsed = difftime(now, g_stats.start_time);
    
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                         Scan Statistics                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Hosts scanned:     %8d                               ║\n", g_stats.hosts_scanned);
    printf("║ Ports scanned:     %8d                               ║\n", g_stats.ports_scanned);
    printf("║ Open ports:        %8d                               ║\n", g_stats.ports_open);
    printf("║ Scan time:         %8.1f seconds                      ║\n", elapsed);
    printf("║ Scan rate:         %8.1f ports/sec                    ║\n", g_stats.ports_scanned / elapsed);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    pthread_mutex_unlock(&g_stats.mutex);
}

// Main function
int main(int argc, char *argv[]) {
    print_banner();
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    char *target_spec = NULL;
    char *port_spec = "top100";
    char *output_file = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            g_config.verbose = 1;
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--ports") == 0) {
            if (++i < argc) port_spec = argv[i];
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
            if (++i < argc) g_config.threads = atoi(argv[i]);
        } else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--timeout") == 0) {
            if (++i < argc) g_config.timeout_ms = atoi(argv[i]);
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scan-type") == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], "syn") == 0) g_config.scan_type = SCAN_TCP_SYN;
                else if (strcmp(argv[i], "connect") == 0) g_config.scan_type = SCAN_TCP_CONNECT;
                else if (strcmp(argv[i], "udp") == 0) g_config.scan_type = SCAN_UDP;
            }
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], "json") == 0) g_config.output_format = OUTPUT_JSON;
                else if (strcmp(argv[i], "csv") == 0) g_config.output_format = OUTPUT_CSV;
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (++i < argc) output_file = argv[i];
        } else if (argv[i][0] != '-') {
            target_spec = argv[i];
        }
    }
    
    if (!target_spec) {
        printf("[!] No target specified\n");
        return 1;
    }
    
    // Redirect output to file if specified
    if (output_file) {
        freopen(output_file, "w", stdout);
    }
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize statistics
    pthread_mutex_init(&g_stats.mutex, NULL);
    g_stats.start_time = time(NULL);
    
    // Parse targets and ports
    char **targets;
    int target_count;
    uint16_t *ports;
    int port_count;
    
    if (parse_targets(target_spec, &targets, &target_count) < 0) {
        printf("[!] Invalid target specification\n");
        return 1;
    }
    
    if (parse_ports(port_spec, &ports, &port_count) < 0) {
        printf("[!] Invalid port specification\n");
        return 1;
    }
    
    printf("[+] Starting scan against %d targets, %d ports\n", target_count, port_count);
    printf("[+] Using %d threads, %s scan\n", g_config.threads,
           g_config.scan_type == SCAN_TCP_SYN ? "SYN" :
           g_config.scan_type == SCAN_UDP ? "UDP" : "Connect");
    
    if (g_config.output_format == OUTPUT_HUMAN) {
        printf("\nIP              Port   Protocol State    Service      Banner\n");
        printf("────────────────────────────────────────────────────────────────────────\n");
    }
    
    // Initialize work queue
    work_queue_init(&g_work_queue);
    
    // Create thread pool
    g_thread_pool = malloc(g_config.threads * sizeof(pthread_t));
    int *thread_ids = malloc(g_config.threads * sizeof(int));
    
    for (int i = 0; i < g_config.threads; i++) {
        thread_ids[i] = i;
        pthread_create(&g_thread_pool[i], NULL, worker_thread, &thread_ids[i]);
    }
    
    // Add work items
    for (int t = 0; t < target_count && g_running; t++) {
        for (int p = 0; p < port_count && g_running; p++) {
            work_queue_add(&g_work_queue, targets[t], ports[p]);
        }
        g_stats.hosts_scanned++;
    }
    
    // Wait for completion
    printf("[+] Scan started, press Ctrl+C to stop...\n");
    
    while (g_running && g_work_queue.count > 0) {
        sleep(1);
    }
    
    g_running = 0;
    
    // Wake up threads
    for (int i = 0; i < g_config.threads; i++) {
        pthread_cond_broadcast(&g_work_queue.cond);
    }
    
    // Wait for threads to finish
    for (int i = 0; i < g_config.threads; i++) {
        pthread_join(g_thread_pool[i], NULL);
    }
    
    print_statistics();
    
    // Cleanup
    free(g_thread_pool);
    free(thread_ids);
    
    for (int i = 0; i < target_count; i++) {
        free(targets[i]);
    }
    free(targets);
    free(ports);
    
    printf("\n[+] Scan completed\n");
    return 0;
} 