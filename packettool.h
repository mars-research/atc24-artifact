#ifndef _RLTEST_PACKETTOOL_H
#define _RLTEST_PACKETTOOL_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define RTE_IPV4_HDR_IHL_MASK   (0x0f)
#define RTE_IPV4_IHL_MULTIPLIER (4)
#define IPPROTO_UDP   17
#define 	RTE_ETHER_ADDR_LEN   6
 struct rte_ether_addr {
     uint8_t addr_bytes[6]; 
 } ;
 typedef uint16_t rte_be16_t; 
 typedef uint32_t rte_be32_t; 
 typedef uint64_t rte_be64_t; 
 typedef uint16_t rte_le16_t; 
 typedef uint32_t rte_le32_t; 
 typedef uint64_t rte_le64_t; 
  struct rte_ether_hdr {
     struct rte_ether_addr dst_addr; 
     struct rte_ether_addr src_addr; 
     rte_be16_t ether_type; 
 };
 struct rte_udp_hdr {
     rte_be16_t src_port;    
     rte_be16_t dst_port;    
     rte_be16_t dgram_len;   
     rte_be16_t dgram_cksum; 
 };
 struct rte_ipv4_hdr {
     __extension__
     union {
         uint8_t version_ihl;    
         struct {
             uint8_t ihl:4;     
             uint8_t version:4;   
         };
     };
     uint8_t  type_of_service;   
     rte_be16_t total_length;    
     rte_be16_t packet_id;       
     rte_be16_t fragment_offset; 
     uint8_t  time_to_live;      
     uint8_t  next_proto_id;     
     rte_be16_t hdr_checksum;    
     rte_be32_t src_addr;        
     rte_be32_t dst_addr;        
 } ;

struct Flow {
  rte_be32_t src_ip;
  rte_be32_t dst_ip;
  rte_be16_t src_port;
  rte_be16_t dst_port;
  rte_be16_t proto;
};

static struct rte_ipv4_hdr *get_ipv4_hdr(const struct rte_ether_hdr *eth_hdr);


static struct  rte_ipv4_hdr* get_packet_rte_ipv4_hdr(
    struct rte_ether_hdr *eth_hdr);
    
static struct  rte_udp_hdr* get_packet_rte_udp_hdr(
    struct rte_ether_hdr *eth_hdr);

static void copy_flow(struct Flow * src, struct Flow* dst);
static bool cmp_flow(struct Flow * src, struct Flow* dst);
static void ipv4_stamp_flow(struct Flow * outgoing_flow, struct rte_ipv4_hdr *ipv4_hdr, struct rte_udp_hdr *udp_hdr);
#endif
