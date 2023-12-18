#pragma once

#include "Domain.hpp"

class NullDomain : public Domain<NullDomain> {
public:
	static constexpr const char *TYPE = "null";

	NullDomain(uint64_t id, std::string name, const char *path);

	void postLoad_impl();
	uint64_t start_impl(uint64_t thread_id);
	void setSlot_impl(uint64_t slot_id, NullDomain &callee);
};
