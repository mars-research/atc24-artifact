#include "NullDomain.hpp"

template class Domain<NullDomain, NullContext>;

NullDomain::NullDomain(NullContext *context, uint64_t id, std::string name, const char *path)
	: Domain(context, id, name, path)
{
}

void NullDomain::postLoad_impl() {
}

uint64_t NullDomain::start_impl(uint64_t thread_id) {
	uint64_t (*f)() = (uint64_t (*)())entry_point;
	return f();
}

void NullDomain::setSlot_impl(uint64_t slot_id, NullDomain &callee) {
	// TODO
}

void NullDomain::setSlotTcb_impl(uint64_t slot_id, void *callee) {
	// TODO
}
