#include <stdint.h>

// allows the compiler to do common subexpression elimination
static inline void *__capability get_call_slot(uint64_t slot_id) {
	void *__capability cap;
	uint64_t scratch;
	asm(
		"adrp %[scratch], _morello_call_slots;"
		"ldr %[cap], [%[scratch]];"
		: [cap] "=C"(cap)
		: [scratch] "r"(scratch)
	);
	return cap;
}

uint64_t call_slot_2(uint64_t slot_id, uint64_t a, uint64_t b) {
	register uint64_t x0 __asm__("x0") = a;
	register uint64_t x1 __asm__("x1") = b;
	asm volatile(
		"blr %[cap];"
		: "=r"(x0)
		: [cap] "C"(get_call_slot(slot_id)), "r"(x0), "r"(x1)
		: "x30", /*"x1",*/ "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18"
	);

	return x0;
}

uint64_t bar(uint64_t a, uint64_t b) {
	return call_slot_2(0, a * 2, b * 2);
}

