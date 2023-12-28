#pragma once

#include "Domain.hpp"
#include "MorelloContext.hpp"
#include "util.hpp"

#define CALL_SLOT_VADDR 0
#define CALL_SLOT_SIZE 16
#define CALL_SLOT_NUM 10

struct SealedTrampoline;
struct InterDomainTrampoline;
struct DomainTcbTrampoline;

// Reserved:
// Exit Trampoline @ 0x0
//
// VAddr:
// Call Slots      @ 0x0 (16 bytes per slot)
class MorelloDomain : public Domain<MorelloDomain, MorelloContext> {
	friend SealedTrampoline;
	friend InterDomainTrampoline;
	friend DomainTcbTrampoline;

public:
	static constexpr const char *TYPE = "morello";

	MorelloDomain(MorelloContext *context, uint64_t id, std::string name, const char *path);
	~MorelloDomain();

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id);
	void setSlot_impl(uint64_t slot_id, MorelloDomain &callee);
	void setSlotTcb_impl(uint64_t slot_id, void *callee);

protected:
	uint64_t getDomainPermMask() const;
	uint64_t getSlot(uint64_t slot_id) const;
};

struct SealedTrampoline {
	MorelloContext *context;
	void *mapped;
	size_t map_size;
	uint64_t cap_offset;

public:
	SealedTrampoline();
	~SealedTrampoline();

	SealedTrampoline(const SealedTrampoline&) = delete; // cannot be copied
	SealedTrampoline(SealedTrampoline&&) = default; // can be moved
	SealedTrampoline& operator=(const SealedTrampoline&) = delete;

	void *__capability cap();

protected:
	void mapTrampoline(void *bin_start, size_t bin_size);
};

// Domain -> Domain
struct InterDomainTrampoline : public SealedTrampoline {
	InterDomainTrampoline(MorelloDomain *from_domain, MorelloDomain *to_domain);
};

// Domain -> TCB
struct DomainTcbTrampoline : public SealedTrampoline {
	DomainTcbTrampoline(MorelloDomain *from_domain, void *callee);
};
