#include "packettool.h"
#include "hashmap.h"
#include <sys/mman.h>

static size_t round_2_64(size_t input){
	return (input/64+1) * 64;
}
void mte_retag_dcgva(void *base_with_tag, size_t size);
static struct maglev_kv_pair * maglev_kv_pairs;
static struct maglev_hashmap map;


static const uint64_t FNV_BASIS = 0xcbf29ce484222325ull;
static const uint64_t FNV_PRIME = 0x100000001b3;
static uint8_t * empty_key;


void init_nf3(){
    maglev_kv_pairs = mmap ( NULL, CAPACITY * sizeof(struct maglev_kv_pair), PROT_READ | PROT_WRITE| PROT_MTE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
  	if( maglev_kv_pairs == MAP_FAILED){ 
		perror("mprotect() failed"); 
		abort(); 
	  } 
    map.pairs = maglev_kv_pairs;
    mte_retag_dcgva(maglev_kv_pairs,round_2_64(CAPACITY * sizeof(struct maglev_kv_pair))); 

    empty_key = mmap ( NULL, KEY_SIZE * sizeof( uint8_t), PROT_READ | PROT_WRITE| PROT_MTE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
  	if( empty_key == MAP_FAILED){ 
		perror("mprotect() failed"); 
		abort(); 
	  } 
    mte_retag_dcgva(empty_key,round_2_64(KEY_SIZE * sizeof( uint8_t))); 
    for(int i = 0; i<KEY_SIZE; i++){
      empty_key[i] = 0;
    }
}

static __inline__ uint64_t fnv_1(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

static __inline__ uint64_t fnv_1_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state *= FNV_PRIME;
		state ^= data[i];
	}
	return state;
}

static __inline__ uint64_t fnv_1a(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

static __inline__ uint64_t fnv_1a_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state ^= data[i];
		state *= FNV_PRIME;
	}
	return state;
}

static __inline__ unsigned int hash_fn(uint64_t x )
{
	return (x * 14695981039346656037ull) >> (64 - 24);
}

// Maglev hashmap
// empty buckets have zero keys
static void maglev_hashmap_insert(struct Flow* key,
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

static struct maglev_kv_pair* maglev_hashmap_get(struct Flow* key)
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

static struct rte_ipv4_hdr *get_ipv4_hdr(const struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  if (eth_hdr->ether_type != 0x0008) {
    return NULL;
  }
  return (struct rte_ipv4_hdr *)((uint8_t *)eth_hdr +
                                 sizeof(struct rte_ether_hdr));
}




static struct  rte_ipv4_hdr* get_packet_rte_ipv4_hdr(struct rte_ether_hdr *eth_hdr) {
  // Get IPv4 header.
  struct rte_ipv4_hdr *ipv4_hdr = get_ipv4_hdr(eth_hdr);
  if (ipv4_hdr == NULL) {
    return NULL;
  }
  return ipv4_hdr;
}

static struct rte_udp_hdr* get_packet_rte_udp_hdr(struct rte_ether_hdr *eth_hdr) {
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

static __inline__ void copy_flow(struct Flow * src, struct Flow* dst){
  dst->src_ip = src->src_ip;
  dst->dst_ip = src->dst_ip;
  dst->src_port = src->src_port;
  dst->dst_port = src->dst_port;
  dst->proto = src->proto;
}

static __inline__ bool cmp_flow(struct Flow * src, struct Flow* dst){
  return dst->src_ip == src->src_ip &&
  dst->dst_ip == src->dst_ip &&
  dst->src_port ==  src->src_port &&
  dst->dst_port ==  src->dst_port &&
  dst->proto ==  src->proto;
}

static __inline__ void ipv4_stamp_flow(struct Flow * outgoing_flow, struct rte_ipv4_hdr *ipv4_hdr, struct rte_udp_hdr *udp_hdr)  {
  // Update the packet with the NATed flow's IP and port.
  ipv4_hdr->src_addr = outgoing_flow->src_ip;
  ipv4_hdr->dst_addr = outgoing_flow->dst_ip;
  udp_hdr->src_port = outgoing_flow->src_port;
  udp_hdr->dst_port = outgoing_flow->dst_port;
}
struct Acl {
  rte_be32_t src_ip;
  rte_be32_t dst_ip;
  rte_be16_t src_port;
  rte_be16_t dst_port;
  // Related not done
  bool drop;

}Acl;

size_t acls_cap = CAPACITY<<2;
size_t acls_count = 1;
struct Acl acls[CAPACITY<<2]__attribute__ ( (section (".domain_C"))) = {
  [0].src_ip = 0, // 10.0.0.1
  [0].dst_ip = 0, // 10.0.0.2
  [0].src_port = 0,
  [0].dst_port = 0,
  [0].drop = false,
};

bool matches(struct Acl * this, struct Flow *flow){
   bool src_ip_matched =
      this->src_ip == 0 || this->src_ip == flow->src_ip;
   bool dst_ip_matched =
      this->dst_ip == 0 || this->dst_ip == flow->dst_ip;
   bool src_port_matched =
      this->src_port == 0 || this->src_port == flow->src_port;
   bool dst_port_matched =
      this->dst_port == 0 || this->dst_port == flow->dst_port;


  return src_ip_matched && dst_ip_matched && src_port_matched &&
    dst_port_matched;
}

size_t process_frames3(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id){
  for (int i = 0; i < nb_rx; i++) {
    // Get packet header.
    struct rte_ether_hdr *eth_hdr = packets[i];

    struct rte_ipv4_hdr *ipv4_hdr = get_packet_rte_ipv4_hdr(eth_hdr);
    struct rte_udp_hdr *udp_hdr = get_packet_rte_udp_hdr(eth_hdr);


    if (ipv4_hdr == NULL || udp_hdr == NULL ){
      return 0;
    }

    // Extract flow.
    struct Flow flow = {.src_ip = ipv4_hdr->src_addr,
         .dst_ip = ipv4_hdr->dst_addr,
         .src_port = udp_hdr->src_port,
         .dst_port = udp_hdr->dst_port,
         .proto = eth_hdr->ether_type};

    bool matched = false;

    // check if the flow is matched directly by the ACL
    for (int i = 0; i< acls_count; i++) {
        struct Acl *acl = &acls[i];

      if (matches(acl, &flow)) {
        matched = true;
        if (!acl->drop) {
          maglev_hashmap_insert(&flow,NULL);
        }
        break;
      }
    }

    if (!matched) {
      // check if the reverse flow is in the cache
      struct Flow reversed_flow = {
        .src_ip = flow.dst_ip,
        .dst_ip = flow.src_ip,
        .src_port = flow.dst_port,
        .dst_port = flow.src_port,
        .proto = flow.proto
      };

      if (maglev_hashmap_get(&reversed_flow) != NULL) {
        matched = true;
      }
    }

    if (!matched) {
      // TODO: we drop the packet here
    }
    asm volatile("" : "+r"(matched));
  }
}