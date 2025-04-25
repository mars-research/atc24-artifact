#ifndef RT_H
#define RT_H

#include "rt-sizes.h"

// A function call.
typedef struct Call {
	// Stack space.
	char stk[STACK_SIZE];

	// Next call.
	struct Call * next;

	// Padding for 16-byte alignment.
 	void *unused;
} Call;

void RT_init();
void MTE_init();

/// Retags a region.
///
/// Both base and size must be multiples of 16.
void mte_retag(void *base_with_tag, size_t size);
void Enable_MTE();
void Enable_MTE_on_page(void* page_start);
void * domain_alloc(size_t sz, int domain);
#endif
