#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#define ARMV8_PMUV3_PERFCTR_BR_RETIRED 0x0021

struct read_format {
	uint64_t nr;
	struct {
		uint64_t value;
		uint64_t id;
	} value;
};

int main() {
	int fd;
	struct perf_event_attr pea;
	struct read_format rf;

	memset(&pea, 0, sizeof(struct perf_event_attr));
	//pea.type = PERF_TYPE_HARDWARE;
	pea.type = PERF_TYPE_RAW;
	pea.size = sizeof(struct perf_event_attr);
	pea.config = ARMV8_PMUV3_PERFCTR_BR_RETIRED;
	pea.disabled = 1;
	pea.exclude_kernel = 1;
	pea.exclude_hv = 1;
	pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

	fd = syscall(__NR_perf_event_open, &pea, 0, -1, -1, 0);

	if (fd <= 0) {
		printf("Failed to open: %s\n", strerror(errno));
		return 1;
	}

	uint64_t id;
	ioctl(fd, PERF_EVENT_IOC_ID, &id);
	printf("   ID: %lu\n", id);

	ioctl(fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
	ioctl(fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);

	for (size_t i = 0; i < 100; ++i) {
		if (rand() < RAND_MAX / 2) {
			asm volatile("nop");
		}
	}

	int bytes = read(fd, &rf, sizeof(rf));
	printf("   ID: %lu\n", rf.value.id);
	printf("Value: %lu\n", rf.value.value);

	return 0;
}
