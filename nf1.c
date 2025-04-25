#include "packettool.h"

static struct rte_ipv4_hdr * get_ipv4_hdr(const struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  if (eth_hdr->ether_type != 0x0008) {
    return NULL;
  }
  return (struct rte_ipv4_hdr *)((uint8_t *)eth_hdr +
                                 sizeof(struct rte_ether_hdr));
}

size_t process_frames1(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id){
    for (int i = 0; i < nb_rx; i++){
        struct rte_ipv4_hdr* ipv4_hdr = get_ipv4_hdr(packets[i]);
    if (ipv4_hdr == NULL) {
     
      continue;
    }
    // Decrement TLL
    ipv4_hdr->time_to_live--;
    }
}