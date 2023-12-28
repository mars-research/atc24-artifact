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

uint64_t hello_tcb() {
	return 123;
}

int main() {
	std::cerr << "Domain type: " << DEFAULT_DOMAIN_TYPE::TYPE << "\n";

	DEFAULT_CONTEXT_TYPE context;

	DEFAULT_DOMAIN_TYPE foo(&context, 0, "foo", "domains/foo.so");
	DEFAULT_DOMAIN_TYPE bar(&context, 1, "bar", "domains/bar.so");
	DEFAULT_DOMAIN_TYPE baz(&context, 2, "baz", "domains/baz.so");

	foo.setSlot(0, bar); // call slot 0 in foo leads to bar
	foo.setSlotTcb(1, (void*)hello_tcb);

	bar.setSlot(0, baz);

	uint64_t thread_id = 0;
	uint64_t ret = foo.start(thread_id);
	printf("foo() -> 0x%lx\n", ret);
}
