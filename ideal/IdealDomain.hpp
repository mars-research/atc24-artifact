#pragma once

#include "Domain.hpp"
#include "IdealContext.hpp"

#define CALL_SLOT_VADDR 0
#define CALL_SLOT_SIZE 8
#define CALL_SLOT_NUM 10

namespace ideal {

struct Trampoline;
struct InterDomainTrampoline;
struct DomainTcbTrampoline;
struct TcbDomainTrampoline;

class IdealDomain : public Domain<IdealDomain, IdealContext> {
	friend Trampoline;
	friend InterDomainTrampoline;
	friend DomainTcbTrampoline;
	friend TcbDomainTrampoline;

public:
	static constexpr const char *TYPE = "ideal";

	IdealDomain(IdealContext *context, uint64_t id, std::string name, const char *path);

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id, uint64_t arg);
	void setSlot_impl(uint64_t slot_id, IdealDomain &callee);
	void setSlotTcb_impl(uint64_t slot_id, void *callee);

protected:
	uint64_t getSlot(uint64_t slot_id) const;
};

struct Trampoline {
	IdealContext *context;
	void *mapped;
	size_t map_size;
	uint64_t cap_offset;

public:
	Trampoline();
	~Trampoline();

	Trampoline(const Trampoline&) = delete; // cannot be copied
	Trampoline(Trampoline&&) = default; // can be moved
	Trampoline& operator=(const Trampoline&) = delete;

	void *ptr();

protected:
	void mapTrampoline(void *bin_start, size_t bin_size);
};

// Domain -> Domain
struct InterDomainTrampoline : public Trampoline {
	InterDomainTrampoline(IdealDomain *from_domain, IdealDomain *to_domain);
};

// Domain -> TCB
struct DomainTcbTrampoline : public Trampoline {
	DomainTcbTrampoline(IdealDomain *from_domain, void *callee);
};

// TCB -> Domain
struct TcbDomainTrampoline : public Trampoline {
	TcbDomainTrampoline(IdealDomain *to_domain);
};

}
