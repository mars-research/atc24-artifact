#include <stddef.h>

#include "rt.h"

#define ITERATIONS 10000000

#if defined(__aarch64__)
static inline uint64_t rdtsc() {
	uint64_t tsc;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
	return tsc;
}
#elif defined(__x86_64__)
static inline uint64_t rdtsc() {
        uint64_t hi, lo;
        __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
        return ((uint64_t)lo | (uint64_t)hi << 32);
}
#else
#error Unsupported platform
#endif

int main(uint64_t depth) {
	//return call_slot_3(0, 1, 0, 0);

	uint64_t start = rdtsc();

	for (size_t i = 0; i < ITERATIONS; ++i) {
		call_slot_3(0, depth, 0, 0);
	}

	uint64_t elapsed = rdtsc() - start;

	return elapsed / ITERATIONS / depth;
}

uint64_t pong(uint64_t depth, uint64_t tsc, uint64_t acc) {
	if (depth > 1) {
		return call_slot_3(0, depth - 1, 0, 0);
	} else {
		return 0;
	}
}
