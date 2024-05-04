#include <iostream>

#ifndef DEFAULT_DOMAIN_HEADER
#define DEFAULT_DOMAIN_HEADER "NullDomain.hpp"
#include "NullDomain.hpp"
#endif

#ifndef DEFAULT_DOMAIN_TYPE
#define DEFAULT_DOMAIN_TYPE NullDomain
#endif

#ifndef DEFAULT_CONTEXT_TYPE
#define DEFAULT_CONTEXT_TYPE NullContext
#endif

#include DEFAULT_DOMAIN_HEADER

#ifdef DEFAULT_DOMAIN_TYPE_MORELLO
uint64_t (*__capability foo_entry)(uint64_t, uint64_t);

uint64_t tcb_main() {
	return foo_entry(2, 2) + 1;
}
#endif

uint64_t hello_tcb() {
	return 123;
}

int main() {
	std::cerr << "Domain type: " << DEFAULT_DOMAIN_TYPE::TYPE << "\n";

	DEFAULT_CONTEXT_TYPE context;

	DEFAULT_DOMAIN_TYPE pong(&context, 0, "pong", "domains/pong-main.so");
	DEFAULT_DOMAIN_TYPE pong1(&context, 0, "pong1", "domains/pong.so");
	DEFAULT_DOMAIN_TYPE pong2(&context, 0, "pong2", "domains/pong.so");
	pong.setSlot(0, pong1);
	pong1.setSlot(0, pong2);
	pong2.setSlot(0, pong1);

	for (uint64_t depth = 1; depth <= 16; ++depth) {
		uint64_t cycles = pong.start(0, depth);
		printf("%lu, %lu\n", depth, cycles);
	}
}
