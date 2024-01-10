#include "NullDomain.hpp"

template class Domain<null::NullDomain, null::NullContext>;

namespace null {

NullDomain::NullDomain(NullContext *context, uint64_t id, std::string name, const char *path)
	: Domain(context, id, name, path)
{
}

void NullDomain::postLoad_impl() {
	// Map call slots
	void *call_slots = mmap(
		(void*)((uint8_t*)load_bias + CALL_SLOT_VADDR),
		page_size.alignUp(CALL_SLOT_SIZE * CALL_SLOT_NUM),
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		-1,
		0
	);
	if (call_slots == MAP_FAILED) {
		std::cerr << "Failed to reserve memory for call slots\n";
		throw std::runtime_error("Failed to reserve memory for call slots");
	}
}

uint64_t NullDomain::start_impl(uint64_t thread_id) {
	uint64_t (*f)() = (uint64_t (*)())entry_point;
	return f();
}

void NullDomain::setSlot_impl(uint64_t slot_id, NullDomain &callee) {
	volatile void **slot_addr = (volatile void**)this->getSlot(slot_id);
	*slot_addr = (void*)callee.entry_point;
}

void NullDomain::setSlotTcb_impl(uint64_t slot_id, void *callee) {
	// TODO
}

uint64_t NullDomain::getSlot(uint64_t slot_id) const {
	return load_bias + CALL_SLOT_VADDR + CALL_SLOT_SIZE * slot_id;
}

}
