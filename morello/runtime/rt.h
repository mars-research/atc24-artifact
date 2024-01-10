#pragma once

#include <stdint.h>

static inline void *__capability get_call_slot(uint64_t slot_id) {
	void *__capability cap;
	uint64_t scratch;
	asm volatile( // !!!!!
		"adrp %[scratch], _morello_call_slots;"
		"add %[scratch], %[scratch], %[slot_id], lsl 4;"
		"ldr %[cap], [%[scratch]];"
		//"brk 0;"
		: [cap] "=C"(cap), [scratch] "=&r"(scratch)
		: [slot_id] "r"(slot_id)
	);
	return cap;
}

uint64_t call_slot_3(uint64_t slot_id, uint64_t a, uint64_t b, uint64_t c) {
	register uint64_t x0 __asm__("x0") = a;
	register uint64_t x1 __asm__("x1") = b;
	register uint64_t x2 __asm__("x2") = c;
	asm volatile(
		"blr %[cap];"
		: "=r"(x0), "=&r"(x1), "=&r"(x2) // !!!
		: [cap] "C"(get_call_slot(slot_id)), "r"(x0), "r"(x1), "r"(x2)
		: "x30", /*"x1", "x2",*/ "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "cc" // !!!
	);

	return x0;
}
