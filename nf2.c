#include "packettool.h"
#include "hashmap.h"

static struct maglev_kv_pair maglev_kv_pairs[CAPACITY]__attribute__ ((aligned (4096))) ;
static struct maglev_hashmap map  = {.pairs = maglev_kv_pairs};


static const uint64_t FNV_BASIS = 0xcbf29ce484222325ull;
static const uint64_t FNV_PRIME = 0x100000001b3;

static __inline__ uint64_t  fnv_1(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

static __inline__ uint64_t fnv_1_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state *= FNV_PRIME;
		state ^= data[i];
	}
	return state;
}

static __inline__ uint64_t  fnv_1a(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

static __inline__ uint64_t  fnv_1a_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state ^= data[i];
		state *= FNV_PRIME;
	}
	return state;
}



static uint8_t empty_key[KEY_SIZE] = { 0 };

static __inline__ unsigned int  hash_fn(uint64_t x )
{
	return (x * 14695981039346656037ull) >> (64 - 24);
}

// Maglev hashmap
// empty buckets have zero keys
static void  maglev_hashmap_insert(struct Flow* key,
				struct Flow* value)
{
	//uint64_t hash = hash_fn(key);
	uint64_t hash = fnv_1((char*)key, sizeof(struct Flow));
	for (uint64_t i = 0; i < CAPACITY; ++i) {
		uint64_t probe = hash + i;
		struct maglev_kv_pair* pair = &map.pairs[probe % CAPACITY];
		if (pair->vaild == false || cmp_flow(&pair->key,key)) {
			copy_flow(key,&pair->key);
			if (value != NULL){
				copy_flow(value,&pair->value);
			}

			pair->vaild = true;
			break;
		}
	}
}

static struct maglev_kv_pair*  maglev_hashmap_get(struct Flow* key)
{
	//uint64_t hash = hash_fn(key);
	uint64_t hash = fnv_1((char*)key, sizeof(struct Flow));
	for (uint64_t i = 0; i < CAPACITY; ++i) {
		uint64_t probe = hash + i;
		struct maglev_kv_pair *pair = &map.pairs[probe % CAPACITY];
		if (pair->vaild == false) {
			return NULL;
		}
		if (cmp_flow(key,&pair->key)) { // hacky, uses zero key as empty marker
			return pair;
		}
	}

	return NULL;
}

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

static struct rte_ipv4_hdr * get_ipv4_hdr(const struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  if (eth_hdr->ether_type != 0x0008) {
    return NULL;
  }
  return (struct rte_ipv4_hdr *)((uint8_t *)eth_hdr +
                                 sizeof(struct rte_ether_hdr));
}




static struct  rte_ipv4_hdr*  get_packet_rte_ipv4_hdr(struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  struct rte_ipv4_hdr *ipv4_hdr = get_ipv4_hdr(eth_hdr);
  if (ipv4_hdr == NULL) {
    return NULL;
  }
  return ipv4_hdr;
}

static struct rte_udp_hdr*  get_packet_rte_udp_hdr(struct rte_ether_hdr *eth_hdr) {
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

static __inline__ void  copy_flow(struct Flow * src, struct Flow* dst){
  dst->src_ip = src->src_ip;
  dst->dst_ip = src->dst_ip;
  dst->src_port = src->src_port;
  dst->dst_port = src->dst_port;
  dst->proto = src->proto;
}

static __inline__ bool  cmp_flow(struct Flow * src, struct Flow* dst){
  return dst->src_ip == src->src_ip &&
  dst->dst_ip == src->dst_ip &&
  dst->src_port ==  src->src_port &&
  dst->dst_port ==  src->dst_port &&
  dst->proto ==  src->proto;
}

static __inline__ void  ipv4_stamp_flow(struct Flow * outgoing_flow, struct rte_ipv4_hdr *ipv4_hdr, struct rte_udp_hdr *udp_hdr)  {
  // Update the packet with the NATed flow's IP and port.
  ipv4_hdr->src_addr = outgoing_flow->src_ip;
  ipv4_hdr->dst_addr = outgoing_flow->dst_ip;
  udp_hdr->src_port = outgoing_flow->src_port;
  udp_hdr->dst_port = outgoing_flow->dst_port;
}



const static size_t MAX_SIZE = 1 << 16;
const static uint16_t MIN_PORT = 1024;
const static uint16_t MAX_PORT = 65535;

static uint16_t next_port_ = MIN_PORT;


size_t  process_frames2(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id){
  for (int i = 0; i < nb_rx; i++) {
    // Get packet header.
    struct rte_ether_hdr *eth_hdr = packets[i];

    struct rte_ipv4_hdr *ipv4_hdr = get_packet_rte_ipv4_hdr(eth_hdr);
    struct rte_udp_hdr *udp_hdr = get_packet_rte_udp_hdr(eth_hdr);

    if (ipv4_hdr == NULL || udp_hdr == NULL ){
      abort();
      return 0;
    }

    // Extract flow.
    struct Flow flow = {.src_ip = ipv4_hdr->src_addr,
         .dst_ip = ipv4_hdr->dst_addr,
         .src_port = udp_hdr->src_port,
         .dst_port = udp_hdr->dst_port,
         .proto = eth_hdr->ether_type};

    // Check if the flow is already NATed.
    struct maglev_kv_pair * pair = maglev_hashmap_get(&flow);
    asm volatile("" : "+r"(pair));
    if (pair != NULL) {
      // Stamp the pack with the outgoing flow.
      struct Flow * outgoing_flow = &pair->value;
      ipv4_stamp_flow(outgoing_flow,ipv4_hdr, udp_hdr);
    } else if (next_port_ < MAX_PORT) {
      // Allocate a new port.
      uint16_t assigned_port = next_port_;
      next_port_++;

      // Update flow_vec
      //flow_vec_[assigned_port] = {flow, 0, true};

      // Create a new outgoing flow.
      struct Flow outgoing_flow = {.src_ip = ipv4_hdr->src_addr,
         .dst_ip = ipv4_hdr->dst_addr,
         .src_port = assigned_port,
         .dst_port = udp_hdr->dst_port,
         .proto = eth_hdr->ether_type};
      //outgoing_flow.src_port = assigned_port;

      // Update flow mapping.
      maglev_hashmap_insert(&flow, &outgoing_flow);

      struct Flow outgoing_flow_r = {.src_ip = outgoing_flow_r.dst_ip,
         .dst_ip = outgoing_flow_r.src_ip,
         .src_port = outgoing_flow_r.dst_port,
         .dst_port = outgoing_flow_r.src_port,
         .proto = outgoing_flow_r.proto};

      struct Flow flow_r = {.src_ip = flow_r.dst_ip,
         .dst_ip = flow_r.src_ip,
         .src_port = flow_r.dst_port,
         .dst_port = flow_r.src_port,
         .proto = flow_r.proto};
      maglev_hashmap_insert(&outgoing_flow_r, &flow_r);

      // Stamp the pack with the outgoing flow.
      ipv4_stamp_flow(&outgoing_flow,ipv4_hdr, udp_hdr);
    }
  }
}