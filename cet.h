#ifndef CET_H
#define CET_H

// Simple header-only utilities for CET
//
// Zhaofeng Li

#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ARCH_PRCTL(code,ptr) ({\
	int ret;\
	asm(\
		"pushfq;"\
		"pop %%r11;"\
		"syscall;"\
		:\
			"=a"(ret)\
		:\
			"a"((uint64_t)SYS_arch_prctl),\
			"D"((uint64_t)code),\
			"S"((uint64_t)ptr)\
		: "r11"\
	);\
	ret;\
})

#define CETMACRO_EMPTY()
#define CETMACRO_DEFER(id) id CETMACRO_EMPTY()
#define CETMACRO_EXPAND(...) __VA_ARGS__

#define ENABLE_CET() {\
	int ret = CETMACRO_EXPAND(CETMACRO_DEFER(ARCH_PRCTL)(ARCH_X86_FEATURE_ENABLE, LINUX_X86_FEATURE_SHSTK));\
	if (ret) {\
		fprintf(stderr, "Failed to enable CET: %d\n", ret);\
		abort();\
	}\
}

#define DISABLE_CET() {\
	int ret = CETMACRO_EXPAND(CETMACRO_DEFER(ARCH_PRCTL)(ARCH_X86_FEATURE_DISABLE, LINUX_X86_FEATURE_SHSTK));\
	if (ret) {\
		fprintf(stderr, "Failed to disable CET: %d\n", ret);\
		abort();\
	}\
}

typedef struct cet_status {
	uint64_t shstk_ibt_status;
	uint64_t shstk_base_addr;
	uint64_t shstk_size;
} cet_status;

static int get_cet_status(cet_status *status) {
	return syscall(SYS_arch_prctl, ARCH_X86_FEATURE_STATUS, status);
}

static void print_cet_status() {
	cet_status status;
	int ret = get_cet_status(&status);

	if (ret) {
		fprintf(stderr, "print_cet_status: syscall returned %d\n", ret);
		return;
	}

	fprintf(stderr, "  shadow stack/IBT status: %lx\n", status.shstk_ibt_status);
	fprintf(stderr, "shadow stack base address: %lx\n", status.shstk_base_addr);
	fprintf(stderr, "        shadow stack size: %lx\n", status.shstk_size);
}

#endif
