#pragma once
#include "cheri.h"

extern "C" {

void *__capability get_ddc_cur();
void store_cap(void *dest, void *__capability cap);

static inline void dump_cap(void *__capability cap) {
	printf("===\n");
	bool tagged = __builtin_cheri_tag_get(cap);
	if (tagged) {
		printf("Capability %p (tagged=%d)\n", (__cheri_fromcap void*)cap, tagged);
	} else {
		printf("BAD Capability %p (tagged=%d) !!!\n", (__cheri_fromcap void*)cap, tagged);
	}
	printf("   Sealed: 0x%d\n", __builtin_cheri_sealed_get(cap));
	printf("  Address: 0x%lx\n", __builtin_cheri_address_get(cap));
	printf("     Base: 0x%lx\n", __builtin_cheri_base_get(cap));
	printf("   Length: 0x%lx\n", __builtin_cheri_length_get(cap));

	uint64_t perms = __builtin_cheri_perms_get(cap);
	printf("    Perms: 0x%lx", perms);
	if (perms & CAP_PERM_GLOBAL) { printf(" Global"); }
	if (perms & CAP_PERM_EXECUTIVE) { printf(" Executive"); }
	if (perms & CAP_PERM_SYSTEM) { printf(" System"); }

	if (perms & CAP_PERM_STORE_CAP) { printf(" StoreCap"); }
	if (perms & CAP_PERM_STORE_LOCAL) { printf(" StoreLocalCap"); }

	if (perms & CAP_PERM_LOAD) { printf(" Load"); }
	if (perms & CAP_PERM_STORE) { printf(" Store"); }
	if (perms & CAP_PERM_EXECUTE) { printf(" Execute"); }

	if (perms & CAP_PERM_LOAD_CAP) { printf(" LoadCap"); }
	if (perms & CAP_PERM_MUTABLE_LOAD) { printf(" MutableLoad"); }

	printf("\n");

	printf("===\n");
}

}
