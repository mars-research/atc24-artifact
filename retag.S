.text

/// Retags a region.
///
/// void retag_region(void *base_with_tag, size_t size);
///
/// Both base and size must be multiples of 16.

.global mte_retag_dcgva
mte_retag_dcgva:
	and x2, x0, 0b111111
	cbz x2, 2f
1:  // -> base is not 64-byte aligned
	brk 0xf1
	b 1b
2:  // -> base is 64-byte aligned
	and x2, x1, 0b111111
	cbz x2, 2f
1:  // -> size is not a multiple of 64
	brk 0xf2
	b 1b
2:  // -> size is a multiple of 64

	add x2, x0, x1 // x2 = end address

1:  // loop
	dc gva, x0
	add x0, x0, 64
	cmp x0, x2
	b.lt 1b
	ret