#pragma once

#include "Context.hpp"
#include "Domain.hpp"

namespace ideal {

class IdealDomain;
struct Trampoline;
struct InterDomainTrampoline;
struct DomainTcbTrampoline;
struct TcbDomainTrampoline;

class IdealContext : public Context<IdealContext> {
	friend IdealDomain;
	friend Trampoline;
	friend DomainTcbTrampoline;
	friend InterDomainTrampoline;
	friend TcbDomainTrampoline;

public:
	uint64_t startTcb_impl(uint64_t thread_id, void *callee);

protected:
	void *tcb_base;
	void setUpTcb();
	uint64_t getStack(uint64_t thread_id) const;
};

}
