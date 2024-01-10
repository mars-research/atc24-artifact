#pragma once

#include <stdint.h>

static inline void ** get_call_base() {
	void **base;
	asm(
		"adrp %0, _ideal_call_slots;"
		: "=r"(base)
	);
	return base;
}

uint64_t call_slot_3(uint64_t slot_id, uint64_t a, uint64_t b, uint64_t c) {
	void **base = get_call_base();
	//printf("base @ %p\n", base);

	uint64_t (*f)(uint64_t, uint64_t, uint64_t) = (uint64_t (*)(uint64_t, uint64_t, uint64_t))(*(base + slot_id));
	//printf("f @ %p\n", f);

	return f(a, b, c);
}
