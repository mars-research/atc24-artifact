#include "IdealContext.hpp"

namespace ideal {

uint64_t IdealContext::startTcb_impl(uint64_t thread_id, void *callee) {
	// TODO
}

void IdealContext::setUpTcb() {
	Alignment page_size(4096);

	tcb_base = mmap(
		nullptr,
		ELF_BASE_OFFSET,
		PROT_NONE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);
	if (tcb_base == MAP_FAILED) {
		throw std::runtime_error("Failed to reserve memory for TCB");
	}

	fprintf(stderr, "[ideal] TCB base: %p\n", tcb_base);

	// Set up stacks
	void *stacks = mmap(
		(void*)((uint8_t*)tcb_base + STACK_OFFSET),
		STACK_SIZE * STACK_NUM,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		-1,
		0
	);
	if (stacks == MAP_FAILED) {
		throw std::runtime_error("Failed to map stacks");
	}

	for (auto i = 0; i < STACK_NUM; ++i) {
		uint64_t stack_top = (uint64_t)tcb_base + STACK_OFFSET + i * STACK_SIZE;
		uint64_t stack_bottom = stack_top + STACK_SIZE;
		uint64_t initial_sp = Alignment(16).alignDown(stack_bottom);

		volatile uint64_t *sp_save = (uint64_t*)stack_top;
		*sp_save = initial_sp;
	}
}

uint64_t IdealContext::getStack(uint64_t thread_id) const {
	return (uint64_t)tcb_base + STACK_OFFSET + thread_id * STACK_SIZE;
}

}
