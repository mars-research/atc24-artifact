#include "packettool.h"

// struct Flow get_flow(const rte_ether_hdr *eth_hdr, const rte_ipv4_hdr *ipv4_hdr,
//        const rte_udp_hdr *udp_hdr){
//   struct Flow flow = {.src_ip = ipv4_hdr->src_addr,
//         .dst_ip = ipv4_hdr->dst_addr,
//         .src_port = udp_hdr->src_port,
//         .dst_port = udp_hdr->dst_port,
//         .proto = eth_hdr->ether_type};
//   return flow;
// }

struct rte_ipv4_hdr *get_ipv4_hdr(const struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  if (eth_hdr->ether_type != 0x0008) {
    return NULL;
  }
  return (struct rte_ipv4_hdr *)((uint8_t *)eth_hdr +
                                 sizeof(struct rte_ether_hdr));
}




struct  rte_ipv4_hdr* get_packet_rte_ipv4_hdr(struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  struct rte_ipv4_hdr *ipv4_hdr = get_ipv4_hdr(eth_hdr);
  if (ipv4_hdr == NULL) {
    return NULL;
  }
  return ipv4_hdr;
}

struct rte_udp_hdr* get_packet_rte_udp_hdr(struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  struct rte_ipv4_hdr *ipv4_hdr = get_ipv4_hdr(eth_hdr);
  if (ipv4_hdr == NULL) {
    return NULL;
  }
  if (ipv4_hdr->next_proto_id != IPPROTO_UDP) {
    return NULL;
  }
  size_t ip_hdr_offset =
      (ipv4_hdr->version_ihl & RTE_IPV4_HDR_IHL_MASK) * RTE_IPV4_IHL_MULTIPLIER;
  struct rte_udp_hdr *udp_hdr =
      (struct rte_udp_hdr *)((uint8_t *)ipv4_hdr + ip_hdr_offset);

  return udp_hdr;
}

__inline__ void copy_flow(struct Flow * src, struct Flow* dst){
  dst->src_ip = src->src_ip;
  dst->dst_ip = src->dst_ip;
  dst->src_port = src->src_port;
  dst->dst_port = src->dst_port;
  dst->proto = src->proto;
}

__inline__ bool cmp_flow(struct Flow * src, struct Flow* dst){
  return dst->src_ip == src->src_ip &&
  dst->dst_ip == src->dst_ip &&
  dst->src_port ==  src->src_port &&
  dst->dst_port ==  src->dst_port &&
  dst->proto ==  src->proto;
}

__inline__ void ipv4_stamp_flow(struct Flow * outgoing_flow, struct rte_ipv4_hdr *ipv4_hdr, struct rte_udp_hdr *udp_hdr)  {
  // Update the packet with the NATed flow's IP and port.
  ipv4_hdr->src_addr = outgoing_flow->src_ip;
  ipv4_hdr->dst_addr = outgoing_flow->dst_ip;
  udp_hdr->src_port = outgoing_flow->src_port;
  udp_hdr->dst_port = outgoing_flow->dst_port;
}

