#include <stddef.h>

#include "rt.h"

#define ITERATIONS 100000
#define DEPTH 16

static inline uint64_t rdtsc() {
	uint64_t tsc;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
	return tsc;
}

int main() {
	uint64_t start = rdtsc();

	for (size_t i = 0; i < ITERATIONS; ++i) {
		call_slot_3(0, DEPTH, 0, 0);
	}

	uint64_t elapsed = rdtsc() - start;

	return elapsed / ITERATIONS / DEPTH;
}

uint64_t pong(uint64_t depth, uint64_t tsc, uint64_t acc) {
	if (depth > 1) {
		return call_slot_3(0, depth - 1, 0, 0);
	} else {
		return 0;
	}
}
