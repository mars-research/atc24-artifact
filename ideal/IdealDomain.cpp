#include "IdealDomain.hpp"

template class Domain<ideal::IdealDomain, ideal::IdealContext>;

namespace ideal {

extern "C" {

extern uint8_t _ideal_trampoline_start;
extern uint8_t _ideal_trampoline_end;

extern uint8_t _ideal_tcb_trampoline_start;
extern uint8_t _ideal_tcb_trampoline_end;

}

static uint64_t align(uint64_t addr, uint64_t alignment) {
	return (addr + alignment - 1) & ~(alignment - 1);
}

IdealDomain::IdealDomain(IdealContext *context, uint64_t id, std::string name, const char *path)
	: Domain(context, id, name, path)
{
}

void IdealDomain::postLoad_impl() {
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

uint64_t IdealDomain::start_impl(uint64_t thread_id) {
	/*
	uint64_t (*f)() = (uint64_t (*)())entry_point;
	return f();
	*/

	uint64_t stack_top = this->getStack(thread_id);
	uint64_t stack_bottom = stack_top + STACK_SIZE;
	uint64_t initial_sp = Alignment(16).alignDown(stack_bottom);

	// RSP
	volatile uint64_t *sp_save = (uint64_t*)stack_top;
	*sp_save = initial_sp;

	//printf("sp @ %lx\n", initial_sp);

#ifdef __aarch64__
	register uint64_t x0 __asm__("x0") = thread_id;
	asm(
		// FIXME: Save to TCB stack!
		"mov x9, sp;"
		"mov sp, %[initial_sp];"
		"str x9, [sp, -16]!;"

		"blr %[entry_point];"

		"ldr x9, [sp], 16;"
		"mov sp, x9;"

		: "=r"(x0)
		: "r"(x0), [entry_point] "r"(entry_point), [initial_sp] "r"(initial_sp)
		: "memory", "x30", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"
	);
	return x0;
#elif defined(__x86_64__)
	register uint64_t rax __asm__("rax");
	asm(
		// FIXME: Save to TCB stack!
		"mov r10, rsp;"
		"mov rsp, %[initial_sp];"
		"push r10;"

		"call %[entry_point];"

		"pop rsp;"

		: "=a"(rax)
		: [entry_point] "r"(entry_point), [initial_sp] "r"(initial_sp)
		: "memory", "cc", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11"
	);
	return rax;
#else
	return 1;
#endif
}

void IdealDomain::setSlot_impl(uint64_t slot_id, IdealDomain &callee) {
	InterDomainTrampoline *tramp = new InterDomainTrampoline(this, &callee);
	void *ptr = tramp->ptr();

	volatile void **slot_addr = (volatile void**)this->getSlot(slot_id);
	*slot_addr = ptr;

	fprintf(stderr, "[ideal] %s %ld (0x%lx) -> 0x%lx -> %s\n",
		name.data(), slot_id, slot_addr, ptr, callee.name.data()
	);
}

void IdealDomain::setSlotTcb_impl(uint64_t slot_id, void *callee) {
	// TODO
}

uint64_t IdealDomain::getSlot(uint64_t slot_id) const {
	return load_bias + CALL_SLOT_VADDR + CALL_SLOT_SIZE * slot_id;
}

Trampoline::Trampoline() {
}

Trampoline::~Trampoline() {
	if (mapped) {
		std::cerr << "[trampoline] Unmapping " << mapped << "\n";
		munmap(mapped, map_size);
	}
}

void Trampoline::mapTrampoline(void *bin_start, size_t bin_size) {
	if (bin_size % 16) {
		throw std::runtime_error(std::string("Trampoline binary not aligned: ") + std::to_string(bin_size));
	}

	if (bin_size > 4096) {
		throw std::runtime_error(std::string("Trampoline binary too big: ") + std::to_string(bin_size));
	}

	cap_offset = align(bin_size, 16);

	map_size = align(cap_offset + 3 * sizeof(void *), 4096);
	mapped = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (mapped == MAP_FAILED) {
		throw std::runtime_error("Failed to reserve trampoline memory");
	}

	memcpy(mapped, bin_start, bin_size);

	fprintf(stderr, "[trampoline] Generated @ %p (bin_size=0x%lx, map_size=0x%lx)\n", mapped, bin_size, map_size);
}

void *Trampoline::ptr() {
	return mapped;
}

// Domain -> Domain
InterDomainTrampoline::InterDomainTrampoline(IdealDomain *from_domain, IdealDomain *to_domain) : Trampoline() {
	if (from_domain->context != to_domain->context) {
		throw std::runtime_error("Cannot link domains with different contexts");
	}
	context = from_domain->context;

	fprintf(stderr, "[trampoline] Domain %s -> Domain %s\n", from_domain->name.data(), to_domain->name.data());

	size_t bin_size = (size_t)&_ideal_trampoline_end - (size_t)&_ideal_trampoline_start;
	this->mapTrampoline((void*)&_ideal_trampoline_start, bin_size);

	void volatile **context_ptr = (void volatile **)((uint8_t*)mapped + cap_offset);
	auto push_ptr = [&] (void *ptr) {
		*context_ptr = ptr;
		context_ptr++;
	};

	// [Trampoline Code][Destination][Caller Base][Callee Base]
	push_ptr((void*)to_domain->entry_point);
	push_ptr((void*)from_domain->base);
	push_ptr((void*)to_domain->base);

	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

// Domain -> TCB
DomainTcbTrampoline::DomainTcbTrampoline(IdealDomain *from_domain, void *callee) : Trampoline() {
	context = from_domain->context;
	fprintf(stderr, "[trampoline] Domain %s -> TCB %p\n", from_domain->name.data(), callee);

	size_t bin_size = (size_t)&_ideal_tcb_trampoline_end - (size_t)&_ideal_tcb_trampoline_start;
	this->mapTrampoline((void*)&_ideal_tcb_trampoline_start, bin_size);

	void volatile **context_ptr = (void volatile **)((uint8_t*)mapped + cap_offset);
	auto push_ptr = [&] (void *ptr) {
		*context_ptr = ptr;
		context_ptr++;
	};

	// [Trampoline Code][Destination][Caller Base][Callee Base]
	push_ptr(callee);
	push_ptr((void*)from_domain->base);
	push_ptr((void*)context->tcb_base);

	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

// TCB -> Domain
TcbDomainTrampoline::TcbDomainTrampoline(IdealDomain *to_domain) : Trampoline() {
	context = to_domain->context;

	fprintf(stderr, "[trampoline] TCB -> Domain %s\n", to_domain->name.data());

	size_t bin_size = (size_t)&_ideal_tcb_trampoline_end - (size_t)&_ideal_tcb_trampoline_start;
	this->mapTrampoline((void*)&_ideal_tcb_trampoline_start, bin_size);

	void volatile **context_ptr = (void volatile **)((uint8_t*)mapped + cap_offset);
	auto push_ptr = [&] (void *ptr) {
		*context_ptr = ptr;
		context_ptr++;
	};

	// [Trampoline Code][Destination][Caller Base][Callee Base]
	push_ptr((void*)to_domain->entry_point);
	push_ptr((void*)context->tcb_base);
	push_ptr((void*)to_domain->base);

	if (mprotect(mapped, map_size, PROT_READ | PROT_EXEC)) {
		throw std::runtime_error("Failed to mprotect trampoline page");
	}
}

}
