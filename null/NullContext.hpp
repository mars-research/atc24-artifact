#pragma once

#include "Context.hpp"

namespace null {

class NullContext : public Context<NullContext> {
public:
	uint64_t startTcb_impl(uint64_t thread_id, void *callee);
};

}
