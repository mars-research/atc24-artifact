#pragma once

#include <stdint.h>

#if defined(__aarch64__)
static inline void ** get_call_base() {
	void **base;
	asm(
		"adrp %0, _null_call_slots;"
		: "=r"(base)
	);
	return base;
}
#elif defined(__x86_64__)
static inline void ** get_call_base() {
	void **base;
	asm(
		"lea %0, [rip + _ideal_call_slots];"
		: "=r"(base)
	);
	return base;
}
#endif

uint64_t call_slot_3(uint64_t slot_id, uint64_t a, uint64_t b, uint64_t c) {
	void **base = get_call_base();
	//printf("base @ %p\n", base);

	uint64_t (*f)(uint64_t, uint64_t, uint64_t) = (uint64_t (*)(uint64_t, uint64_t, uint64_t))(*(base + slot_id));
	//printf("f @ %p\n", f);

	return f(a, b, c);
}
