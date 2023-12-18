#pragma once

#include "Domain.hpp"

#define CALL_SLOT_VADDR 0
#define CALL_SLOT_SIZE 16
#define CALL_SLOT_NUM 10

struct SealedTrampoline;

// Reserved:
// Exit Trampoline @ 0x0
//
// VAddr:
// Call Slots      @ 0x0 (16 bytes per slot)
class MorelloDomain : public Domain<MorelloDomain> {
	friend SealedTrampoline;

public:
	static constexpr const char *TYPE = "morello";

	MorelloDomain(uint64_t id, std::string name, const char *path);

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id);
	void setSlot_impl(uint64_t slot_id, MorelloDomain &callee);

protected:
	uint64_t getSlot(uint64_t slot_id) const;
};

struct SealedTrampoline {
	void *mapped;
	size_t map_size;

public:
	SealedTrampoline(MorelloDomain *from_domain, MorelloDomain *to_domain);
	~SealedTrampoline();

	// cannot be copied
	SealedTrampoline(const SealedTrampoline&) = delete;
	SealedTrampoline& operator=(const SealedTrampoline&) = delete;

	void *__capability cap();
};
