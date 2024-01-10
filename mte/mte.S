.text

// void mte_retag_{stg,st2g,dcgva}(void *base_with_tag, size_t size);

.global mte_retag_stg
mte_retag_stg:
	and x2, x0, 0b1111
	cbz x2, 2f
1:  // -> base is not 16-byte aligned
	brk 0xf1
	b 1b
2:  // -> base is 16-byte aligned
	and x2, x1, 0b1111
	cbz x2, 2f
1:  // -> size is not a multiple of 16
	brk 0xf2
	b 1b
2:  // -> size is a multiple of 16

	add x2, x0, x1 // x2 = end address

1:  // loop
	stg x0, [x0]
	add x0, x0, 16
	cmp x0, x2
	b.lt 1b
	ret

.global mte_retag_st2g
mte_retag_st2g:
	and x2, x0, 0b11111
	cbz x2, 2f
1:  // -> base is not 32-byte aligned
	brk 0xf1
	b 1b
2:  // -> base is 32-byte aligned
	and x2, x1, 0b11111
	cbz x2, 2f
1:  // -> size is not a multiple of 32
	brk 0xf2
	b 1b
2:  // -> size is a multiple of 32

	add x2, x0, x1 // x2 = end address

1:  // loop
	st2g x0, [x0]
	add x0, x0, 32
	cmp x0, x2
	b.lt 1b
	ret


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
