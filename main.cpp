#include <iostream>

#ifndef DEFAULT_DOMAIN_HEADER
#define DEFAULT_DOMAIN_HEADER "NullDomain.hpp"
#include "NullDomain.hpp"
#endif

#ifndef DEFAULT_DOMAIN_TYPE
#define DEFAULT_DOMAIN_TYPE NullDomain
#endif

#include DEFAULT_DOMAIN_HEADER

int main() {
	std::cerr << "Domain type: " << DEFAULT_DOMAIN_TYPE::TYPE << "\n";

	DEFAULT_DOMAIN_TYPE foo(0, "foo", "domains/foo.so");
	DEFAULT_DOMAIN_TYPE bar(1, "bar", "domains/bar.so");
	DEFAULT_DOMAIN_TYPE baz(2, "baz", "domains/baz.so");

	foo.setSlot(0, bar); // call slot 0 in foo leads to bar
	bar.setSlot(0, baz);

	uint64_t thread_id = 0;
	uint64_t ret = foo.start(thread_id);
	printf("foo() -> %ld\n", ret);
}
