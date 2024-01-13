#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <time.h>

#define ITERATIONS 10000
#define TOTAL_SIZE (4ULL * 1024 * 1024 * 1024) // 4 GiB

void mte_retag_stg(void *base_with_tag, size_t size);
void mte_retag_st2g(void *base_with_tag, size_t size);
void mte_retag_dcgva(void *base_with_tag, size_t size);

static inline uint64_t rdtsc() {
	uint64_t tsc;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
	return tsc;
}

static inline uint64_t cntvct() {
	uint64_t tsc;
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

static inline size_t black_box(size_t val) {
#pragma GCC push_options
#pragma GCC optimize ("O0")
	return val;
#pragma GCC pop_options
}

//printf("%6s: %.0f, ~%.2f cycles, ~%.2f cycles/page\n", name, per_iteration, cycles, cycles_per_page);
#define CASE_CNTVCT(name, size, body) {\
	size_t num_mb = size / (1ULL * 1024 * 1024);\
	size_t num_pages = size / 4096; \
	size_t iterations = black_box(TOTAL_SIZE / size); \
	uint64_t start = cntvct();\
	for (size_t i = 0; i < iterations; ++i) {\
		body;\
	}\
	uint64_t end = cntvct();\
	uint64_t elapsed = end - start; \
	float per_iteration = (float)elapsed / iterations; \
	float cycles = per_iteration * ratio; \
	float cycles_per_page = cycles / num_pages; \
	printf("%s, %lu, %lu, %.0f, %.2f\n", name, size, iterations, cycles, cycles_per_page);\
}

//printf("%lu iterations, %lu, %lu\n", iterations, TOTAL_SIZE, size);
#define CASE(name, size, body) {\
	size_t num_mb = size / (1ULL * 1024 * 1024);\
	size_t num_pages = size / 4096; \
	size_t iterations = black_box(TOTAL_SIZE / size); \
	uint64_t start = rdtsc();\
	for (size_t i = 0; i < iterations; i++) {\
		body;\
	}\
	uint64_t end = rdtsc();\
	uint64_t elapsed = end - start; \
	float per_iteration = (float)elapsed / iterations; \
	float cycles = per_iteration; \
	float cycles_per_page = cycles / num_pages; \
	printf("%s, %lu, %lu, %.0f, %.2f\n", name, size, iterations, cycles, cycles_per_page);\
}

int main() {
	enable_mte();

	uint64_t cpu_hz = 2914000000;
	uint64_t cpu_mhz = 2914;

	uint64_t timer_hz;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(timer_hz));

	float ratio = (float)cpu_hz / (float)timer_hz;

	/*
	printf("  CPU: %lu Hz\n", cpu_hz);
	printf("Timer: %lu Hz\n", timer_hz);
	printf("Ratio: %f\n", ratio);
	*/

	//for (uint64_t size = 4ULL * 1024 * 1024; size <= 32ULL * 1024 * 1024; size *= 2) {
	for (uint64_t size = (1ULL << 4); size <= (1ULL << 25); size <<= 1) {
		fprintf(stderr, "## %lu MiB, %lu\n", size / 1024 / 1024, size);

		void *buf = aligned_alloc(4096, size);
		void *buf2 = aligned_alloc(4096, size);

		if (!buf) { abort(); }
		if (!buf2) { abort(); }

		memset(buf2, 2, size);
		memset(buf, 1, size);
		protect_mte(buf, size);

		volatile void (*my_memcpy)(void *, void *, uint64_t);
		my_memcpy = (volatile void (*)(void *, void *, uint64_t)) memcpy;

		CASE("memcpy", size, my_memcpy(buf2, buf, size));
		if (size >= 16) {
			CASE("stg", size, mte_retag_stg(buf, size));
		}
		if (size >= 32) {
			CASE("st2g", size, mte_retag_st2g(buf, size));
		}
		if (size >= 64) {
			CASE("dc gva", size, mte_retag_dcgva(buf, size));
		}

		free(buf);
		free(buf2);

		fprintf(stderr, "\n");
	}
}
