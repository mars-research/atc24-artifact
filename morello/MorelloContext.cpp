#include "MorelloContext.hpp"

extern uint8_t _morello_rets_trampoline_start;
extern uint8_t _morello_rets_trampoline_end;

MorelloContext::MorelloContext() {
	std::cerr << "[morello] Created context\n";

	use_restricted_mode = true;

	this->setUpTcb();
}

MorelloContext::~MorelloContext() {
	if (tcb_base) {
		munmap(tcb_base, ELF_BASE_OFFSET);
	}
}

void MorelloContext::setUpTcb() {
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

	fprintf(stderr, "[morello] TCB base: %p\n", tcb_base);

	// Set up stacks
	void *stacks = mmap(
		(void*)((uint8_t*)tcb_base + STACK_OFFSET),
		STACK_SIZE * STACK_NUM,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_GROWSDOWN,
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

	// Put rets trampoline at page 0
	uint64_t map_size = page_size.alignment;
	void *rets_trampoline = mmap(
		tcb_base,
		map_size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		-1,
		0
	);

	if (rets_trampoline == MAP_FAILED) {
		throw std::runtime_error("Failed to map RETS trampoline page");
	}

	uint64_t bin_size = (uint64_t)&_morello_rets_trampoline_end - (uint64_t)&_morello_rets_trampoline_start;
	void *bin_start = (void*)&_morello_rets_trampoline_start;

	if (bin_size > map_size) {
		throw std::runtime_error("RETS trampoline too big");
	}

	memcpy(rets_trampoline, bin_start, bin_size);

	if (mprotect(rets_trampoline, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect RETS trampoline");
	}
}

uint64_t MorelloContext::getDomainPermMask() const {
	if (use_restricted_mode) {
		return CAP_PERM_ALL & ~CAP_PERM_EXECUTIVE;
	} else {
		return CAP_PERM_ALL;
	}
}

uint64_t MorelloContext::getStack(uint64_t thread_id) const {
	return (uint64_t)tcb_base + STACK_OFFSET + thread_id * STACK_SIZE;
}
