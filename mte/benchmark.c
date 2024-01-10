#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <time.h>

#define ITERATIONS 10000

void mte_retag_stg(void *base_with_tag, size_t size);
void mte_retag_st2g(void *base_with_tag, size_t size);
void mte_retag_dcgva(void *base_with_tag, size_t size);

static inline uint64_t rdtsc() {
	uint64_t tsc;
	//asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
	asm volatile("mrs %0, cntvct_el0" : "=r"(tsc));
	return tsc;
}

static inline uint64_t nanos() {
	struct timespec v;
	clock_gettime(CLOCK_MONOTONIC_RAW, &v);
	return v.tv_sec * 1000000 + v.tv_nsec;
}

static void enable_mte() {
	if (prctl(PR_SET_TAGGED_ADDR_CTRL,
			PR_TAGGED_ADDR_ENABLE | PR_MTE_TCF_SYNC | PR_MTE_TCF_ASYNC |
			(0xfffe << PR_MTE_TAG_SHIFT),
			0, 0, 0)) {
		perror("prctl() failed");
		abort();
	}
}

static void protect_mte(void* start, size_t size){
	if (mprotect(start, size, PROT_READ | PROT_WRITE | PROT_MTE)) {
		perror("mprotect() failed");
		abort();
	}
}

#define CASE(name, size, body) {\
	size_t num_pages = size / 4096;\
	uint64_t start = rdtsc();\
	for (size_t i = 0; i < ITERATIONS; ++i) {\
		body;\
	}\
	uint64_t elapsed = rdtsc() - start;\
	printf("%6s: %lu cycles, %lu cycles/page\n", name, elapsed, elapsed / ITERATIONS / num_pages);\
}

int main() {
	enable_mte();

	for (uint64_t size = 4ULL * 1024 * 1024; size <= 16ULL * 1024 * 1024; size *= 2) {
		printf("## %lu MiB\n", size / 1024 / 1024);

		void *buf = aligned_alloc(4096, size);
		void *buf2 = aligned_alloc(4096, size);

		if (!buf) { abort(); }
		if (!buf2) { abort(); }

		memset(buf2, 2, size);
		memset(buf, 1, size);
		protect_mte(buf, size);

		CASE("stg", size, mte_retag_stg(buf, size));
		CASE("st2g", size, mte_retag_st2g(buf, size));
		CASE("dc gva", size, mte_retag_dcgva(buf, size));

		free(buf);
		free(buf2);

		printf("\n");
	}
}
