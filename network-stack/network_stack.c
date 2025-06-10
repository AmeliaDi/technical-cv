/*
 * AmeliaNet - Userspace TCP/IP Stack with DPDK
 * Author: Amelia Enora Marceline Chavez Barroso
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NETWORK_VERSION "1.0.0"

// Ethernet header
struct eth_header {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
} __attribute__((packed));

// IP header
struct ip_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed));

// TCP header
struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_ptr;
} __attribute__((packed));

// Packet processing
void process_packet(uint8_t *packet, size_t len)
{
    struct eth_header *eth = (struct eth_header *)packet;
    
    if (ntohs(eth->ethertype) == 0x0800) { // IPv4
        struct ip_header *ip = (struct ip_header *)(packet + sizeof(struct eth_header));
        
        if (ip->protocol == 6) { // TCP
            struct tcp_header *tcp = (struct tcp_header *)((uint8_t *)ip + (ip->version_ihl & 0xF) * 4);
            
            printf("AmeliaNet: TCP packet %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d\n",
                   (ip->src_ip >> 0) & 0xFF, (ip->src_ip >> 8) & 0xFF,
                   (ip->src_ip >> 16) & 0xFF, (ip->src_ip >> 24) & 0xFF,
                   ntohs(tcp->src_port),
                   (ip->dst_ip >> 0) & 0xFF, (ip->dst_ip >> 8) & 0xFF,
                   (ip->dst_ip >> 16) & 0xFF, (ip->dst_ip >> 24) & 0xFF,
                   ntohs(tcp->dst_port));
        }
    }
}

int main(void)
{
    printf("AmeliaNet - Userspace TCP/IP Stack v%s\n", NETWORK_VERSION);
    printf("High-performance packet processing with DPDK\n");
    
    // Initialize DPDK (simplified)
    printf("Initializing DPDK...\n");
    
    // Main packet processing loop
    printf("Starting packet processing...\n");
    
    return 0;
} 