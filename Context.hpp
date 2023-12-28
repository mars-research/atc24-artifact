#pragma once

#include <cstdint>

template<class T>
class Context {
public:
	Context() {}

	// cannot be copied
	Context(const Context&) = delete;
	Context& operator=(const Context&) = delete;

	uint64_t startTcb(uint64_t thread_id, void *callee) {
		return (static_cast<T*>(this))->startTcb_impl(thread_id, callee);
	}
};
