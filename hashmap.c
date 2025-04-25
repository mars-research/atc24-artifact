// Hash map by David Detweiler and Dan Appel

#include "hashmap.h"
#include <rte_prefetch.h>


struct maglev_kv_pair maglev_kv_pairs[CAPACITY]__attribute__ ((aligned (4096)));
static struct maglev_hashmap map = {.pairs = maglev_kv_pairs};


const uint64_t FNV_BASIS = 0xcbf29ce484222325ull;
const uint64_t FNV_PRIME = 0x100000001b3;

__inline__ uint64_t fnv_1(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

__inline__ uint64_t fnv_1_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state *= FNV_PRIME;
		state ^= data[i];
	}
	return state;
}

__inline__ uint64_t fnv_1a(char *data, size_t len) {
	return fnv_1_multi(data, len, FNV_BASIS);
}

__inline__ uint64_t fnv_1a_multi(char *data, size_t len, uint64_t state) {
	for (size_t i = 0; i < len; ++i) {
		state ^= data[i];
		state *= FNV_PRIME;
	}
	return state;
}



uint8_t empty_key[KEY_SIZE] = { 0 };

__inline__ static unsigned int hash_fn(uint64_t x )
{
	return (x * 14695981039346656037ull) >> (64 - 24);
}

// Maglev hashmap
// empty buckets have zero keys
void maglev_hashmap_insert(struct Flow* key,
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

struct maglev_kv_pair* maglev_hashmap_get(struct Flow* key)
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

