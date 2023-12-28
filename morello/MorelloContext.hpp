#pragma once

#include <cstdint>
#include <iostream>
#include "Context.hpp"
#include "Domain.hpp"
#include "cheri.h"

class MorelloDomain;
struct SealedTrampoline;
struct SealedTcbTrampoline;

class MorelloContext : public Context<MorelloContext> {
	friend MorelloDomain;
	friend SealedTrampoline;
	friend SealedTcbTrampoline;

public:
	MorelloContext();
	~MorelloContext();

protected:
	void *tcb_base;
	bool use_restricted_mode;

	void setUpTcb();
	uint64_t getDomainPermMask() const;
};
