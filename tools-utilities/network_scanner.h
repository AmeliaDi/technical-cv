/*
 * Advanced Network Scanner - Header File
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#ifndef NETWORK_SCANNER_H
#define NETWORK_SCANNER_H

#include <stdint.h>
#include <pthread.h>
#include <time.h>

// Constants
#define MAX_IP_LEN          16
#define MAX_HOSTNAME_LEN    256
#define MAX_SERVICE_LEN     32
#define MAX_BANNER_LEN      256
#define MAX_THREADS         1000
#define DEFAULT_TIMEOUT     1000

// Scan types
typedef enum {
    SCAN_TCP_SYN = 0,
    SCAN_TCP_CONNECT,
    SCAN_UDP,
    SCAN_TCP_FIN,
    SCAN_TCP_XMAS,
    SCAN_TCP_NULL,
    SCAN_TCP_ACK,
    SCAN_TCP_WINDOW
} scan_type_t;

// Output formats
typedef enum {
    OUTPUT_HUMAN = 0,
    OUTPUT_JSON,
    OUTPUT_XML,
    OUTPUT_CSV
} output_format_t;

// Port states
typedef enum {
    PORT_OPEN = 0,
    PORT_CLOSED,
    PORT_FILTERED,
    PORT_OPEN_FILTERED,
    PORT_CLOSED_FILTERED
} port_state_t;

// Scanner configuration
typedef struct {
    int verbose;
    int threads;
    int timeout_ms;
    scan_type_t scan_type;
    output_format_t output_format;
    int rate_limit;
    int enable_stealth;
    int resolve_hostnames;
    int os_detection;
    int version_detection;
    int script_scan;
} scanner_config_t;

// Scan result
typedef struct {
    char ip[MAX_IP_LEN];
    char hostname[MAX_HOSTNAME_LEN];
    uint16_t port;
    int protocol; // IPPROTO_TCP or IPPROTO_UDP
    port_state_t state;
    char service[MAX_SERVICE_LEN];
    char version[MAX_SERVICE_LEN];
    char banner[MAX_BANNER_LEN];
    int ttl;
    double response_time;
} scan_result_t;

// Work item for thread pool
typedef struct work_item {
    char target_ip[MAX_IP_LEN];
    uint16_t port;
    struct work_item *next;
} work_item_t;

// Work queue
typedef struct {
    work_item_t *head;
    work_item_t *tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} work_queue_t;

// Scanner statistics
typedef struct {
    int hosts_scanned;
    int ports_scanned;
    int ports_open;
    int ports_closed;
    int ports_filtered;
    time_t start_time;
    pthread_mutex_t mutex;
} scanner_stats_t;

// Host information
typedef struct {
    char ip[MAX_IP_LEN];
    char hostname[MAX_HOSTNAME_LEN];
    char os[64];
    char os_version[64];
    int alive;
    double avg_response_time;
    int open_ports_count;
    uint16_t *open_ports;
} host_info_t;

// Network interface information
typedef struct {
    char name[16];
    char ip[MAX_IP_LEN];
    char netmask[MAX_IP_LEN];
    char broadcast[MAX_IP_LEN];
    int is_up;
} interface_info_t;

// Function prototypes

// Core scanning functions
scan_result_t tcp_syn_scan(const char *target_ip, uint16_t port, int sockfd);
scan_result_t tcp_connect_scan(const char *target_ip, uint16_t port);
scan_result_t udp_scan(const char *target_ip, uint16_t port);
scan_result_t tcp_fin_scan(const char *target_ip, uint16_t port);
scan_result_t tcp_xmas_scan(const char *target_ip, uint16_t port);
scan_result_t tcp_null_scan(const char *target_ip, uint16_t port);

// Service detection
void detect_service(scan_result_t *result);
int probe_service(const char *ip, uint16_t port, char *service, char *version);
int grab_banner(const char *ip, uint16_t port, char *banner, size_t banner_size);

// OS detection
int detect_os(const char *target_ip, char *os, char *version);
int tcp_fingerprint(const char *target_ip, char *fingerprint);
int icmp_fingerprint(const char *target_ip, char *fingerprint);

// Host discovery
int ping_sweep(const char *network, char ***alive_hosts, int *count);
int arp_scan(const char *network, char ***alive_hosts, int *count);
int tcp_ping(const char *target_ip, uint16_t *ports, int port_count);

// Utility functions
uint16_t calculate_checksum(uint16_t *ptr, int nbytes);
int create_raw_socket(void);
int set_socket_timeout(int sockfd, int timeout_ms);
double get_timestamp(void);

// Target parsing
int parse_targets(const char *target_spec, char ***targets, int *count);
int parse_cidr(const char *cidr, char ***targets, int *count);
int parse_ip_range(const char *range, char ***targets, int *count);
int resolve_hostname(const char *hostname, char *ip);

// Port parsing
int parse_ports(const char *port_spec, uint16_t **ports, int *count);
int expand_port_range(const char *range, uint16_t **ports, int *count);

// Work queue management
void work_queue_init(work_queue_t *queue);
void work_queue_add(work_queue_t *queue, const char *target_ip, uint16_t port);
work_item_t* work_queue_get(work_queue_t *queue);
void work_queue_destroy(work_queue_t *queue);

// Thread management
void* worker_thread(void *arg);
int start_thread_pool(int thread_count);
void stop_thread_pool(void);

// Output functions
void print_scan_result(const scan_result_t *result);
void print_host_info(const host_info_t *host);
void print_statistics(void);
void print_banner(void);
void print_usage(const char *program_name);

// Format converters
const char* port_state_string(port_state_t state);
const char* scan_type_string(scan_type_t type);
void format_json_result(const scan_result_t *result, char *output);
void format_xml_result(const scan_result_t *result, char *output);
void format_csv_result(const scan_result_t *result, char *output);

// Network utilities
int get_local_ip(char *ip);
int get_default_gateway(char *gateway);
int get_network_interfaces(interface_info_t **interfaces, int *count);
int is_local_network(const char *ip);

// Packet crafting
int craft_syn_packet(char *packet, const char *src_ip, const char *dst_ip, 
                    uint16_t src_port, uint16_t dst_port);
int craft_udp_packet(char *packet, const char *src_ip, const char *dst_ip,
                    uint16_t src_port, uint16_t dst_port, const char *data);
int craft_icmp_packet(char *packet, const char *dst_ip, int type, int code);

// Stealth techniques
int randomize_source_port(void);
void randomize_scan_order(uint16_t *ports, int count);
void add_decoy_traffic(const char *target_ip);
int spoof_source_ip(char *spoofed_ip);

// Timing and rate limiting
void adaptive_timing(double *delay, int success_rate);
void implement_rate_limit(int packets_per_second);
double calculate_rtt(const char *target_ip);

// Error handling
void handle_scanner_error(const char *function, int error_code);
int check_privileges(void);
int validate_target(const char *target);
int validate_port_range(uint16_t start, uint16_t end);

// Configuration
int load_config_file(const char *filename, scanner_config_t *config);
int save_results(const char *filename, scan_result_t *results, int count);
int load_port_list(const char *filename, uint16_t **ports, int *count);

// Statistics and reporting
void update_scan_stats(const scan_result_t *result);
void generate_scan_report(const char *filename);
double calculate_scan_efficiency(void);
void log_scan_activity(const char *message);

// Advanced features
int detect_firewall(const char *target_ip);
int detect_load_balancer(const char *target_ip, uint16_t port);
int traceroute_scan(const char *target_ip, int max_hops);
int detect_nat(const char *target_ip);

#endif // NETWORK_SCANNER_H 