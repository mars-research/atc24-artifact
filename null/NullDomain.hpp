#pragma once

#include "Domain.hpp"
#include "NullContext.hpp"

namespace null {

class NullDomain : public Domain<NullDomain, NullContext> {
public:
	static constexpr const char *TYPE = "null";

	NullDomain(NullContext *context, uint64_t id, std::string name, const char *path);

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id);
	void setSlot_impl(uint64_t slot_id, NullDomain &callee);
	void setSlotTcb_impl(uint64_t slot_id, void *callee);
};

}
