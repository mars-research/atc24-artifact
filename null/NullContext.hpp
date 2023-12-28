#pragma once

#include "Context.hpp"

class NullContext : public Context<NullContext> {
public:
	uint64_t startTcb_impl(uint64_t thread_id, void *callee);
};
