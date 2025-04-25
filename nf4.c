#include "maglev.h"
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>

static size_t round_2_64(size_t input){
	return (input/64+1) * 64;
}
void mte_retag_dcgva(void *base_with_tag, size_t size);
static const uint64_t ETH_HEADER_LEN __attribute__ ( (section (".rodata_D")))= 14;
static const uint64_t UDP_HEADER_LEN __attribute__ ( (section (".rodata_D")))= 8;

// https://en.wikipedia.org/wiki/IPv4
static const uint64_t IPV4_PROTO_OFFSET __attribute__ ( (section (".rodata_D")))= 9;
static const uint64_t IPV4_LENGTH_OFFSET __attribute__ ( (section (".rodata_D")))= 2;
static const uint64_t IPV4_CHECKSUM_OFFSET __attribute__ ( (section (".rodata_D")))= 10;
static const uint64_t IPV4_SRCDST_OFFSET __attribute__ ( (section (".rodata_D")))= 12;
static const uint64_t IPV4_SRCDST_LEN __attribute__ ( (section (".rodata_D")))= 8;
static const uint64_t UDP_LENGTH_OFFSET __attribute__ ( (section (".rodata_D")))= 4;
static const uint64_t UDP_CHECKSUM_OFFSET __attribute__ ( (section (".rodata_D")))= 6;

#define TABLE_SIZE 65537

typedef int Node;
typedef int8_t LookUpTable[TABLE_SIZE];

// MAGLEV: Connection tracking table
static struct maglev_kv_pair * maglev_kv_pairs;
static struct maglev_hashmap map;

// MAGLEV: Lookup table
static LookUpTable maglev_lookup __attribute__ ( (section (".domain_D")));

static const uint64_t FNV_BASIS __attribute__ ( (section (".rodata_D"))) = 0xcbf29ce484222325ull;
static const uint64_t FNV_PRIME __attribute__ ( (section (".rodata_D")))= 0x100000001b3;



static __inline__ uint64_t __attribute__ ( (section (".text_D")))  fnv_1_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state *= FNV_PRIME;
		state ^= data[i];
	}
	return state;
}
static __inline__ uint64_t __attribute__ ( (section (".text_D"))) fnv_1(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}
static __inline__ uint64_t __attribute__ ( (section (".text_D"))) fnv_1a(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

static __inline__ uint64_t __attribute__ ( (section (".text_D"))) fnv_1a_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state ^= data[i];
		state *= FNV_PRIME;
	}
	return state;
}
static void __attribute__ ( (section (".text_D"))) swap_mac(struct rte_ether_hdr *eth_hdr) {
  uint8_t 	tmp;
  for(int i = 0; i < RTE_ETHER_ADDR_LEN; i++){
    tmp = eth_hdr->src_addr.addr_bytes[i];
    eth_hdr->src_addr.addr_bytes[i] = eth_hdr->dst_addr.addr_bytes[i];
    eth_hdr->dst_addr.addr_bytes[i]= tmp;
  }
}

static __inline__ uint64_t __attribute__ ( (section (".text_D"))) flowhash(void *frame, size_t len) {
	static int packets = 0;
	// Warning: This implementation returns 0 for invalid inputs
	char *f = (char*)frame;

	if ((len > 0) && (packets < 256)) {
		//packets++;
		//printf("len %d\n", len);
		//hexdump(f, len);
	}
	// Fail early
	if (f[ETH_HEADER_LEN] >> 4 != 4) {
        // This shitty implementation can only handle IPv4 :(
		printf("unhandled! not ipv4?\n");
		return 0;
	}

	char proto = f[ETH_HEADER_LEN + IPV4_PROTO_OFFSET];
	if (proto != 6 && proto != 17) {
        // This shitty implementation can only handle TCP and UDP
		printf("Unhandled proto %x\n", proto);
		return 0;
	}

	size_t v4len = 4 * (f[ETH_HEADER_LEN] & 0b1111);
	if (len < (ETH_HEADER_LEN + v4len + 4)) {
		// Not long enough
		//printf("header len short len %d expected %d\n", len, (ETH_HEADER_LEN + v4len + 4));
		//hexdump(f, 64);
		return 0;
	}

	uint64_t hash = FNV_BASIS;

    // Hash source/destination IP addresses
	hash = fnv_1_multi(f + ETH_HEADER_LEN + IPV4_SRCDST_OFFSET, IPV4_SRCDST_LEN, hash);

	// Hash IP protocol number
	hash = fnv_1_multi(f + ETH_HEADER_LEN + IPV4_PROTO_OFFSET, 1, hash);

    // Hash source/destination port
	hash = fnv_1_multi(f + ETH_HEADER_LEN + v4len, 4, hash);

	return hash;
}


__inline__ static void __attribute__ ( (section (".text_D"))) get_offset_skip(Node node, size_t *offset, size_t *skip) {
	*offset = fnv_1((void*)&node, sizeof(node)) % (TABLE_SIZE - 1) + 1;
	*skip = fnv_1a((void*)&node, sizeof(node)) % TABLE_SIZE;
}

__inline__ static size_t __attribute__ ( (section (".text_D"))) get_permutation(Node node, size_t j) {
	size_t offset, skip;
	get_offset_skip(node, &offset, &skip);
	return (offset + j * skip) % TABLE_SIZE;
}

// Eisenbud 3.4
static void __attribute__ ( (section (".text_D"))) populate_lut(LookUpTable lut) {
    maglev_kv_pairs = mmap ( NULL, CAPACITY * sizeof(struct maglev_kv_pair), PROT_READ | PROT_WRITE| PROT_MTE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
  	if( maglev_kv_pairs == MAP_FAILED){ 
		perror("mprotect() failed"); 
		abort(); 
	  } 
    map.pairs = maglev_kv_pairs;
	mte_retag_dcgva(maglev_kv_pairs,round_2_64(CAPACITY * sizeof(struct maglev_kv_pair))); 

	// The nodes are meaningless, just like my life
	Node nodes[3] = {800, 273, 8255};
	size_t next[3] = {0, 0, 0};

	for (size_t i = 0; i < TABLE_SIZE; ++i) {
		lut[i] = -1;
	}

	size_t n = 0;

	for (;;) {
		for (size_t i = 0; i < 3; ++i) {
			size_t c = get_permutation(nodes[i], next[i]);
			while (lut[c] >= 0) {
				next[i]++;
				c = get_permutation(nodes[i], next[i]);
			}

			lut[c] = i;
			next[i]++;
			n++;

			if (n == TABLE_SIZE) return;
		}
	}
}

static void __attribute__ ( (section (".text_D"))) maglev_hashmap_insert(uint64_t key, uint64_t value)
{
	//uint64_t hash = hash_fn(key);
	uint64_t hash = fnv_1((char*)&key, sizeof(key));
	for (uint64_t i = 0; i < CAPACITY; ++i) {
		uint64_t probe = hash + i;
		struct maglev_kv_pair* pair = &map.pairs[probe % CAPACITY];
		if (pair->key == key || pair->key == 0) {
			pair->key = key;
			pair->value = value;
			break;
		}
	}
}

static struct maglev_kv_pair* __attribute__ ( (section (".text_D"))) maglev_hashmap_get( uint64_t key)
{
	//uint64_t hash = hash_fn(key);
	uint64_t hash = fnv_1((char*)&key, sizeof(key));
	for (uint64_t i = 0; i < CAPACITY; ++i) {
		uint64_t probe = hash + i;
		struct maglev_kv_pair *pair = &map.pairs[probe % CAPACITY];
		if (pair->key == 0) {
			return NULL;
		}
		if (pair->key == key) { // hacky, uses zero key as empty marker
			return pair;
		}
	}

	return NULL;
}



static int64_t __attribute__ ( (section (".text_D"))) maglev_process_frame(struct rte_ether_hdr *frame) {
	int64_t backend = -1;
	uint64_t hash = flowhash((void*)frame, 1500);
	if (hash != 0) {
		struct maglev_kv_pair *cached = maglev_hashmap_get(hash);
		if (cached == NULL) {
			// Use lookup table
			backend = maglev_lookup[hash % TABLE_SIZE];
			maglev_hashmap_insert( hash, backend);
		} else {
			backend = cached->value;
			// Just use the cached backend (noop in this test)
		}

	}
	return backend;
}

void __attribute__ ( (section (".text_D"))) nf4_init(void) {

	populate_lut(maglev_lookup);
}

size_t __attribute__ ( (section (".text_D"))) process_frames4(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id){
  for (int i = 0; i < nb_rx; i++) {
    // Get packet header.
    struct rte_ether_hdr *eth_hdr = packets[i];
    int64_t backend = maglev_process_frame(eth_hdr);
    if (backend) {
      eth_hdr->dst_addr.addr_bytes[5] = (char)backend;
    }
  }
}