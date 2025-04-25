#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <sys/prctl.h>

void Enable_MTE(){
	if (prctl(PR_SET_TAGGED_ADDR_CTRL,
			PR_TAGGED_ADDR_ENABLE | PR_MTE_TCF_SYNC | PR_MTE_TCF_ASYNC |
			(0xfffe << PR_MTE_TAG_SHIFT),
			0, 0, 0)) {
		perror("prctl() failed");
		abort();
	}
			// printf("MTE enabled!\n");
}

void Enable_MTE_on_page(void* page_start){
	unsigned long page_sz = sysconf(_SC_PAGESIZE);
	if (mprotect(page_start, page_sz, PROT_READ | PROT_WRITE | PROT_MTE  )) {
		perror("mprotect() failed");
		abort();
	}
}

int main(int argc, char * argv[]) {
    	if (argc != 2) {
		fprintf(stderr, "Usage: %s  [buffer size]", argv[0]);
		return 1;
		}

    Enable_MTE();

    size_t buffer_size = atoi(argv[1]);
	struct timespec start_ts, end_ts;

	if (buffer_size % 16){
		fprintf(stderr, "Usage: %s  [buffer size (multiply of 16) ]", argv[0]);
		return 1;
	}

	clock_gettime(CLOCK_MONOTONIC, &start_ts);

    char * buffer_a = aligned_alloc(4096,buffer_size);
    char * buffer_b = aligned_alloc(4096,buffer_size);

    char * buffer_c = aligned_alloc(4096,buffer_size);
    Enable_MTE_on_page(buffer_c);
    size_t iterations = 1000000;

    clock_gettime(CLOCK_MONOTONIC, &start_ts);
	for (size_t i = 0; i < iterations; ++i) {
		volatile void *buffer_a_cp;
		asm volatile ("mov %0, %1" : "=r"(buffer_a_cp) : "r"(buffer_a));
        memcpy(buffer_a_cp,buffer_b,buffer_size);
	}
	clock_gettime(CLOCK_MONOTONIC, &end_ts);
	uint64_t millis = (end_ts.tv_sec - start_ts.tv_sec) * 1000 + (end_ts.tv_nsec - start_ts.tv_nsec) / 1000000;
	uint64_t mspb_cp =  millis;
	// printf("%zu, %zu\n", buffer_size, mspb_cp);
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
	for (size_t i = 0; i < iterations; ++i) {
        mte_retag( buffer_c,buffer_size);
	}

	clock_gettime(CLOCK_MONOTONIC, &end_ts);
	millis = (end_ts.tv_sec - start_ts.tv_sec) * 1000 + (end_ts.tv_nsec - start_ts.tv_nsec) / 1000000;
	uint64_t mspb_tag =  millis;

	// printf("%zu, %zu, %zu\n", buffer_size, mspb_cp, mspb_tag);
	printf("%zu, %zu\n", mspb_cp, mspb_tag);

}