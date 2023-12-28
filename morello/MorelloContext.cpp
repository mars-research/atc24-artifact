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

uint64_t MorelloContext::startTcb_impl(uint64_t thread_id, void *callee) {
	uint64_t stack_top = this->getStack(thread_id);
	uint64_t stack_bottom = stack_top + STACK_SIZE;
	uint64_t initial_sp = Alignment(16).alignDown(stack_bottom);

	// RSP
	volatile uint64_t *sp_save = (uint64_t*)stack_top;
	*sp_save = initial_sp;
	asm volatile("msr rsp_el0, %0" :: "r"(initial_sp));

	// CID
	//
	// Currently contains the thread ID
	void *__capability cid_cap = get_ddc_cur();
	cid_cap = __builtin_cheri_address_set(cid_cap, thread_id);
	cid_cap = __builtin_cheri_perms_and(cid_cap, CAP_PERM_COMPARTMENT_ID);
	cid_cap = __builtin_cheri_seal_entry(cid_cap);
	asm volatile("msr cid_el0, %0" :: "C"(cid_cap));

	// RDDC
	void *__capability tcb_cap = get_ddc_cur();

	tcb_cap = __builtin_cheri_perms_and(tcb_cap, this->getDomainPermMask());
	tcb_cap = __builtin_cheri_address_set(tcb_cap, (uint64_t)tcb_base);
	asm volatile("msr rddc_el0, %0" :: "C"(tcb_cap));

	// Sealed entry
	//
	// Here we jump to the exit trampoline at the first page
	tcb_cap = __builtin_cheri_address_set(tcb_cap, (uint64_t)tcb_base); // TODO
	tcb_cap = __builtin_cheri_seal_entry(tcb_cap);

	if (!use_restricted_mode) {
		std::cerr << "[morello] Staying in Executive Mode\n";

		register uint64_t x0 __asm__("x0") = thread_id;
		asm(
			// FIXME: Save to TCB stack!
			"mov x9, sp;"
			"mov sp, %[initial_sp];"
			"str x9, [sp, -16]!;"

			"mrs c9, ddc;"
			"sub sp, sp, 16;"
			"str c9, [sp];"

			"mov x9, %[entry_point];"

			"blr %[callee];"

			"ldr c9, [sp];"
			"msr ddc, c9;"
			"add sp, sp, 16;"

			"ldr x9, [sp], 16;"
			"mov sp, x9;"

			: "=r"(x0)
			: "r"(x0), [callee] "C"(tcb_cap), [entry_point] "r"(callee), [initial_sp] "r"(initial_sp)
			: "memory", "x30", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"
		);
		return x0;
	} else {
		std::cerr << "[morello] Entering Restricted Mode\n";

		register uint64_t x0 __asm__("x0") = thread_id;
		asm(
			"mov x9, %[entry_point];"
			"blrr %[callee];"

			: "=r"(x0)
			: "r"(x0), [callee] "C"(tcb_cap), [entry_point] "r"(callee)
			: "memory", "x30", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"
		);
		return x0;
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
