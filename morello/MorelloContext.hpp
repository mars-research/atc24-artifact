#pragma once

#include <cstdint>
#include <iostream>
#include "Context.hpp"
#include "Domain.hpp"
#include "util.hpp"

namespace morello {

class MorelloDomain;
struct SealedTrampoline;
struct InterDomainTrampoline;
struct DomainTcbTrampoline;
struct TcbDomainTrampoline;

class MorelloContext : public Context<MorelloContext> {
	friend MorelloDomain;
	friend SealedTrampoline;
	friend DomainTcbTrampoline;
	friend InterDomainTrampoline;
	friend TcbDomainTrampoline;

public:
	MorelloContext();
	~MorelloContext();

	uint64_t startTcb_impl(uint64_t thread_id, void *callee);

protected:
	void *tcb_base;
	bool trampoline_in_executive;
	bool use_restricted_mode;

	void setUpTcb();
	uint64_t getDomainPermMask() const;
	uint64_t getStack(uint64_t thread_id) const;
};

}
