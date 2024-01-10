#pragma once

#include "Domain.hpp"
#include "NullContext.hpp"

#define CALL_SLOT_VADDR 0
#define CALL_SLOT_SIZE 8
#define CALL_SLOT_NUM 10

namespace null {

class NullDomain : public Domain<NullDomain, NullContext> {
public:
	static constexpr const char *TYPE = "null";

	NullDomain(NullContext *context, uint64_t id, std::string name, const char *path);

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id);
	void setSlot_impl(uint64_t slot_id, NullDomain &callee);
	void setSlotTcb_impl(uint64_t slot_id, void *callee);

protected:
	uint64_t getSlot(uint64_t slot_id) const;
};

}
