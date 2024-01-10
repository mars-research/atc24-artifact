#include "MorelloDomain.hpp"

template class Domain<morello::MorelloDomain, morello::MorelloContext>;

namespace morello {

extern "C" {

extern uint8_t _morello_trampoline_start;
extern uint8_t _morello_trampoline_end;

extern uint8_t _morello_tcb_trampoline_start;
extern uint8_t _morello_tcb_trampoline_end;

extern uint8_t _morello_executive_trampoline_start;
extern uint8_t _morello_executive_trampoline_end;

extern uint8_t _morello_executivetcb_trampoline_start;
extern uint8_t _morello_executivetcb_trampoline_end;

extern uint8_t _morello_rets_trampoline_start;
extern uint8_t _morello_rets_trampoline_end;

extern uint8_t _morello_alt_rets_trampoline_start;
extern uint8_t _morello_alt_rets_trampoline_end;

}

static uint64_t align(uint64_t addr, uint64_t alignment) {
	return (addr + alignment - 1) & ~(alignment - 1);
}

MorelloDomain::MorelloDomain(MorelloContext *context, uint64_t id, std::string name, const char *path)
	: Domain(context, id, name, path)
{
}

MorelloDomain::~MorelloDomain() {
}

void MorelloDomain::postLoad_impl() {
	// Put rets trampoline at page 0
	uint64_t map_size = page_size.alignment;
	void *rets_trampoline = mmap(
		(void*)base,
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

	/*
	if (name == "bar") {
		bin_size = (uint64_t)&_morello_alt_rets_trampoline_end - (uint64_t)&_morello_alt_rets_trampoline_start;
		bin_start = (void*)&_morello_alt_rets_trampoline_start;
	}
	*/

	if (bin_size > map_size) {
		throw std::runtime_error("RETS trampoline too big");
	}

	memcpy(rets_trampoline, bin_start, bin_size);

	if (mprotect(rets_trampoline, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect RETS trampoline");
	}

	// Map call slots
	void *call_slots = mmap(
		(void*)((uint8_t*)load_bias + CALL_SLOT_VADDR),
		page_size.alignUp(CALL_SLOT_SIZE * CALL_SLOT_NUM),
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		-1,
		0
	);
	if (call_slots == MAP_FAILED) {
		std::cerr << "Failed to reserve memory for call slots\n";
		throw std::runtime_error("Failed to reserve memory for call slots");
	}
}

uint64_t MorelloDomain::start_impl(uint64_t thread_id) {
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
	void *__capability domain_cap = get_ddc_cur();

	domain_cap = __builtin_cheri_perms_and(domain_cap, context->getDomainPermMask());
	domain_cap = __builtin_cheri_address_set(domain_cap, (uint64_t)base);
	domain_cap = __builtin_cheri_bounds_set(domain_cap, DOMAIN_SIZE);
	asm volatile("msr rddc_el0, %0" :: "C"(domain_cap));

	// Sealed entry
	//
	// Here we jump to the exit trampoline at the first page
	domain_cap = __builtin_cheri_address_set(domain_cap, (uint64_t)base); // TODO
	domain_cap = __builtin_cheri_seal_entry(domain_cap);

	if (!context->use_restricted_mode) {
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
			: "r"(x0), [callee] "C"(domain_cap), [entry_point] "r"(entry_point), [initial_sp] "r"(initial_sp)
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
			: "r"(x0), [callee] "C"(domain_cap), [entry_point] "r"(entry_point)
			: "memory", "x30", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"
		);
		return x0;
	}

	/*
	register uint64_t x0 __asm__("x0") = thread_id;
	asm(
		"mov x9, sp;"
		"mov sp, %[stack];" // switch to new stack
		"str x9, [sp, 16]!;"
		"blr %[callee];"
		"mov x0, sp;"
		"ldr x9, [sp], 16;" // restore old stack
		"mov sp, x9;"
		: "=r"(x0)
		: "r"(x0), [stack] "r"(initial_sp), [callee] "r"(entry_point)
		: "memory", "x30", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"
	);
	return x0;
	*/
}

void MorelloDomain::setSlot_impl(uint64_t slot_id, MorelloDomain &callee) {
	InterDomainTrampoline *tramp = new InterDomainTrampoline(this, &callee);
	void *__capability cap = tramp->cap();

	auto slot_addr = this->getSlot(slot_id);
	asm volatile("str %0, [%1]" :: "C"(cap), "r"(slot_addr));

	fprintf(stderr, "[morello] %s %ld (0x%lx) -> 0x%lx -> %s\n",
		name.data(), slot_id, slot_addr, __builtin_cheri_address_get(cap), callee.name.data()
	);
}

void MorelloDomain::setSlotTcb_impl(uint64_t slot_id, void *callee) {
	DomainTcbTrampoline *tramp = new DomainTcbTrampoline(this, callee);
	void *__capability cap = tramp->cap();

	auto slot_addr = this->getSlot(slot_id);
	asm volatile("str %0, [%1]" :: "C"(cap), "r"(slot_addr));

	fprintf(stderr, "[morello] %s %ld (0x%lx) -> 0x%lx -> TCB %p\n",
		name.data(), slot_id, slot_addr, __builtin_cheri_address_get(cap), callee
	);
}

uint64_t MorelloDomain::getSlot(uint64_t slot_id) const {
	return load_bias + CALL_SLOT_VADDR + CALL_SLOT_SIZE * slot_id;
}

SealedTrampoline::SealedTrampoline() {
}

SealedTrampoline::~SealedTrampoline() {
	if (mapped) {
		std::cerr << "[trampoline] Unmapping " << mapped << "\n";
		munmap(mapped, map_size);
	}
}

void SealedTrampoline::mapTrampoline(void *bin_start, size_t bin_size) {
	if (bin_size % 16) {
		throw std::runtime_error(std::string("Trampoline binary not aligned: ") + std::to_string(bin_size));
	}

	if (bin_size > 4096) {
		throw std::runtime_error(std::string("Trampoline binary too big: ") + std::to_string(bin_size));
	}

	cap_offset = align(bin_size, 16);

	map_size = align(cap_offset + 3 * sizeof(void *__capability), 4096);
	mapped = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (mapped == MAP_FAILED) {
		throw std::runtime_error("Failed to reserve trampoline memory");
	}

	memcpy(mapped, bin_start, bin_size);

	fprintf(stderr, "[trampoline] Generated @ %p (bin_size=0x%lx, map_size=0x%lx)\n", mapped, bin_size, map_size);
}

void *__capability SealedTrampoline::cap() {
	void *__capability entry_cap = get_ddc_cur();

	if (context->trampoline_in_executive) {
		entry_cap = __builtin_cheri_address_set(entry_cap, (uint64_t)mapped);
		entry_cap = __builtin_cheri_bounds_set(entry_cap, map_size);
		entry_cap = __builtin_cheri_seal_entry(entry_cap);
	} else {
		entry_cap = __builtin_cheri_perms_and(entry_cap, context->getDomainPermMask());
		entry_cap = __builtin_cheri_address_set(entry_cap, (uint64_t)mapped);
		entry_cap = __builtin_cheri_bounds_set(entry_cap, map_size);
		entry_cap = __builtin_cheri_address_set(entry_cap, (uint64_t)mapped | 0x1); // purecap
		entry_cap = __builtin_cheri_seal_entry(entry_cap);
	}

	return entry_cap;
}

// Domain -> Domain
InterDomainTrampoline::InterDomainTrampoline(MorelloDomain *from_domain, MorelloDomain *to_domain) : SealedTrampoline() {
	if (from_domain->context != to_domain->context) {
		throw std::runtime_error("Cannot link domains with different contexts");
	}
	context = from_domain->context;

	fprintf(stderr, "[trampoline] Domain %s -> Domain %s\n", from_domain->name.data(), to_domain->name.data());

	size_t bin_size;

	if (context->trampoline_in_executive) {
		bin_size = (size_t)&_morello_executive_trampoline_end - (size_t)&_morello_executive_trampoline_start;
		this->mapTrampoline((void*)&_morello_executive_trampoline_start, bin_size);
	} else {
		bin_size = (size_t)&_morello_trampoline_end - (size_t)&_morello_trampoline_start;
		this->mapTrampoline((void*)&_morello_trampoline_start, bin_size);
	}

	void *__capability *context_ptr = (void *__capability *)((uint8_t*)mapped + cap_offset);
	auto push_capability = [&] (void *__capability cap) {
		store_cap(context_ptr, cap);
		context_ptr++;
	};

	// [Trampoline Code][Super Cap][Caller Cap][Callee Cap]

	{
		void *__capability super_cap = get_ddc_cur();
		super_cap = __builtin_cheri_address_set(super_cap, to_domain->entry_point);
		push_capability(super_cap);
	}

	{
		void *__capability caller_cap = get_ddc_cur();
		caller_cap = __builtin_cheri_perms_and(caller_cap, context->getDomainPermMask());
		caller_cap = __builtin_cheri_address_set(caller_cap, from_domain->base);
		caller_cap = __builtin_cheri_bounds_set(caller_cap, from_domain->size);
		push_capability(caller_cap);
	}

	{
		void *__capability callee_cap = get_ddc_cur();
		callee_cap = __builtin_cheri_perms_and(callee_cap, context->getDomainPermMask());
		callee_cap = __builtin_cheri_address_set(callee_cap, to_domain->base);
		callee_cap = __builtin_cheri_bounds_set(callee_cap, to_domain->size);
		push_capability(callee_cap);
	}


	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

// Domain -> TCB
DomainTcbTrampoline::DomainTcbTrampoline(MorelloDomain *from_domain, void *callee) : SealedTrampoline() {
	context = from_domain->context;
	fprintf(stderr, "[trampoline] Domain %s -> TCB %p\n", from_domain->name.data(), callee);

	size_t bin_size;

	if (context->trampoline_in_executive) {
		bin_size = (size_t)&_morello_executivetcb_trampoline_end - (size_t)&_morello_executivetcb_trampoline_start;
		this->mapTrampoline((void*)&_morello_executivetcb_trampoline_start, bin_size);
	} else {
		bin_size = (size_t)&_morello_tcb_trampoline_end - (size_t)&_morello_tcb_trampoline_start;
		this->mapTrampoline((void*)&_morello_tcb_trampoline_start, bin_size);
	}

	void *__capability *context_ptr = (void *__capability *)((uint8_t*)mapped + cap_offset);
	auto push_capability = [&] (void *__capability cap) {
		store_cap(context_ptr, cap);
		context_ptr++;
	};

	// [Trampoline Code][Super Cap][Caller Cap][Callee Cap]

	{
		void *__capability super_cap = get_ddc_cur();
		super_cap = __builtin_cheri_address_set(super_cap, (uint64_t)callee);
		push_capability(super_cap);
	}

	{
		void *__capability caller_cap = get_ddc_cur();
		caller_cap = __builtin_cheri_perms_and(caller_cap, context->getDomainPermMask());
		caller_cap = __builtin_cheri_address_set(caller_cap, from_domain->base);
		caller_cap = __builtin_cheri_bounds_set(caller_cap, from_domain->size);
		push_capability(caller_cap);
	}

	{
		void *__capability callee_cap = get_ddc_cur();
		callee_cap = __builtin_cheri_perms_and(callee_cap, context->getDomainPermMask());
		callee_cap = __builtin_cheri_address_set(callee_cap, (uint64_t)context->tcb_base);
		push_capability(callee_cap);
	}


	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

// TCB -> Domain
//
// TODO: caller is executive
TcbDomainTrampoline::TcbDomainTrampoline(MorelloDomain *to_domain) : SealedTrampoline() {
	context = to_domain->context;

	fprintf(stderr, "[trampoline] TCB -> Domain %s\n", to_domain->name.data());

	size_t bin_size;

	if (context->trampoline_in_executive) {
		bin_size = (size_t)&_morello_executive_trampoline_end - (size_t)&_morello_executive_trampoline_start;
		this->mapTrampoline((void*)&_morello_executive_trampoline_start, bin_size);
	} else {
		bin_size = (size_t)&_morello_trampoline_end - (size_t)&_morello_trampoline_start;
		this->mapTrampoline((void*)&_morello_trampoline_start, bin_size);
	}

	void *__capability *context_ptr = (void *__capability *)((uint8_t*)mapped + cap_offset);
	auto push_capability = [&] (void *__capability cap) {
		store_cap(context_ptr, cap);
		context_ptr++;
	};

	// [Trampoline Code][Super Cap][Caller Cap][Callee Cap]

	{
		void *__capability super_cap = get_ddc_cur();
		super_cap = __builtin_cheri_address_set(super_cap, to_domain->entry_point);
		push_capability(super_cap);
	}

	{
		void *__capability caller_cap = get_ddc_cur();
		caller_cap = __builtin_cheri_perms_and(caller_cap, context->getDomainPermMask());
		caller_cap = __builtin_cheri_address_set(caller_cap, (uint64_t)context->tcb_base);
		push_capability(caller_cap);
	}

	{
		void *__capability callee_cap = get_ddc_cur();
		callee_cap = __builtin_cheri_perms_and(callee_cap, context->getDomainPermMask());
		callee_cap = __builtin_cheri_address_set(callee_cap, to_domain->base);
		callee_cap = __builtin_cheri_bounds_set(callee_cap, to_domain->size);
		push_capability(callee_cap);
	}


	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

}
