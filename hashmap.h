#ifndef _RLTEST_HASHMAP_H
#define _RLTEST_HASHMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include <stddef.h>
#include <stdint.h>

#include "packettool.h"


static uint64_t fnv_1(char *data, size_t len);
static uint64_t fnv_1_multi(char *data, size_t len, uint64_t state);
static uint64_t fnv_1a(char *data, size_t len);
static uint64_t fnv_1a_multi(char *data, size_t len, uint64_t state);

#define CAPACITY ((1ULL << 20) * 16)

/* maglev KV pair */
struct maglev_kv_pair {
    struct Flow key;
    struct Flow value;
    bool vaild;
};

#define KEY_SIZE	64
#define VALUE_SIZE	64


struct maglev_hashmap {
    struct maglev_kv_pair *pairs;
};

static struct maglev_kv_pair* maglev_hashmap_get(struct Flow* key);

#endif
