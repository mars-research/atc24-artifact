.text

// We target ARMv9-A.

// https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers
// x0-x7: arguments and return value
// x8-18: temporary registers
// x19-x28: callee-saved
// x29/fp: frame pointer
// x30/lr: link register

#define _😰👉👉 #
_😰👉👉 "[31;1;4m((You must run this file through the preprocessor))[0m"
#include "rt-sizes.h"

// Thread- and domain-local variables
.extern RT_CALLER_SAVED_SP
.extern RT_CALLER_SAVED_SP_NEXT
.extern RT_CALLER_SAVED_SP_MAX
.extern RT_CALLEE_STACK

// Global variables
.extern RT_FREE_LIST

.extern RT_error

// Creates a new trampoline.
//
// Example: make_trampoline foo
// Creates a new function `foo` that invokes `_foo`.
.macro make_trampoline name
	make_trampoline_with \name _\name
.endm // .macro make_trampoline

// Creates a new trampoline with an arbitrary unwrapped function.
//
// Example: make_trampoline_with foo real_foo
// Creates a new function `foo` that invokes `real_foo`.
.macro make_trampoline_with name unwrapped
.global \name
\name:
	trampoline_enter
	bl \unwrapped
	trampoline_exit
	ret
.endm // .macro make_trampoline_with

// The first part of the trampoline.
//
// Intended to be used together with `trampoline_exit` as:
//     trampoline_enter
//     bl unwrapped_func
//     trampoline_exit
//     ret
.macro trampoline_enter
	// [x19][x20][x21][x22][x23][x24][x25][x26][x27][x28][fp/x29][original lr/x30]
	sub sp, sp, 8*12

	// save callee-saved registers and lr
	save_or_restore_on_stack stp

	// ====================================================
	// address space switch happens here (caller -> kernel)
	// ====================================================

	// save caller sp
	ldr x8, RT_CALLER_SAVED_SP_NEXT
	ldr x9, RT_CALLER_SAVED_SP_MAX
	ldr x10, =RT_CALLER_SAVED_SP_NEXT
	cmp x8, x9
	bls 2f

1:  // -> sp save space not available
	brk 0xf1
	b 1b

2:  // -> sp save space available
	mov x11, sp
	str x11, [x8], 8 // post-increment, store then add x8, x8, 8
	str x8, [x10]

	// wipe registers for confidentiality
	// domains are mutually untrusting
	//
	// we pass through the argument registers
	mov x8, 0
	mov x9, 0
	mov x10, 0
	mov x11, 0
	mov x12, 0
	mov x13, 0
	mov x14, 0
	mov x15, 0
	mov x16, 0
	mov x17, 0
	mov x18, 0

	mov x19, 0
	mov x20, 0
	mov x21, 0
	mov x22, 0
	mov x23, 0
	mov x24, 0
	mov x25, 0
	mov x26, 0
	mov x27, 0
	mov x28, 0

	mov fp, 0

	// ====================================================
	// address space switch happens here (kernel -> callee)
	// ====================================================

1:  // try to obtain new stack space
	ldr x8, =RT_FREE_LIST // x8 is the top of the new stack
	ldr x8, [x8]
	cbnz x8, 3f

2:  // -> stack space not available
	brk 0xf2
	b 2b

3:  // -> stack space available
	add x9, x8, CALL_NEXT // x9 is .next, also the bottom of the new stack

	// try to remove from free list
	ldr x10, [x9] // read .next
	ldr x11, =RT_FREE_LIST
	mov x12, x8
	casa x12, x10, [x11] // expected, desired, ptr
	// cas sets expected to actual
	cmp x12, x8
	bne 1b // expected != actual

	// set new sp
	mov sp, x9

	// set .next, add to stack
	ldr x11, =RT_CALLEE_STACK
	ldr x10, [x11]
	str x10, [x9] // new_stack->next = previous_callee_stack
	str x8, [x11] // RT_CALLEE_STACK = new_stack

	// we are ready to call the target

.endm // .macro trampoline_enter

// The second part of the trampoline.
.macro trampoline_exit
	ldr x11, =RT_CALLEE_STACK
	ldr x10, [x11] // callee_stack = RT_CALLEE_STACK
	cbnz x10, 2f

1:  // -> RT_CALLEE_STACK == 0
	brk 0xf3
	b 1b

2:  // -> RT_CALLEE_STACK != 0
	add x13, x10, CALL_NEXT // callee_stack->next
	ldr x12, [x13] // previous_callee_stack = callee_stack->next
	str x12, [x11] // RT_CALLEE_STACK = previous_callee_stack

1:  // try to reinsert to free list
	ldr x9, =RT_FREE_LIST
	ldr x8, [x9]
	str x8, [x13] // callee_stack->next = RT_FREE_LIST
	mov x11, x8
	casa x11, x10, [x9] // expected, desired, ptr
	// cas sets expected to actual
	cmp x11, x8
	bne 1b // expected != actual

	// ====================================================
	// address space switch happens here (callee -> kernel)
	// ====================================================

	// wipe registers for confidentiality
	// domains are mutually untrusting

	// we assume the function only returns one value
	mov x1, 0xbeef
	mov x2, 0xbeef
	mov x3, 0xbeef
	mov x4, 0xbeef
	mov x5, 0xbeef
	mov x6, 0xbeef
	mov x7, 0xbeef

	// preserve x8/xr (must be in shared heap to be accessible)
	mov x9, 0
	mov x10, 0
	mov x11, 0
	mov x12, 0
	mov x13, 0
	mov x14, 0
	mov x15, 0
	mov x16, 0
	mov x17, 0
	mov x18, 0

	mov x19, 0
	mov x20, 0
	mov x21, 0
	mov x22, 0
	mov x23, 0
	mov x24, 0
	mov x25, 0
	mov x26, 0
	mov x27, 0
	mov x28, 0

	mov fp, 0

	// no registers are trusted here except x0

	// restore caller sp
	ldr x8, RT_CALLER_SAVED_SP_NEXT
	ldr x9, =RT_CALLER_SAVED_SP_NEXT
	ldr x10, [x8, -8]! // pre-increment
	mov sp, x10
	str x8, [x9]

	// ====================================================
	// address space switch happens here (kernel -> caller)
	// ====================================================

	// restore callee-saved registers and lr
	save_or_restore_on_stack ldp

	add sp, sp, 8*12
.endm // .macro trampoline_exit

// Saves or restores registers on the stack.
//
// To save: save_or_restore_on_stack stp
// To restore: save_or_restore_on_stack ldp
.macro save_or_restore_on_stack pair_op
	// [x19][x20][x21][x22][x23][x24][x25][x26][x27][x28][fp/x29][original lr/x30]

	// save/restore x19~x29 and x30/lr in pairs
	\pair_op x19, x20, [sp, 8* 0]
	\pair_op x21, x22, [sp, 8* 2]
	\pair_op x23, x24, [sp, 8* 4]
	\pair_op x25, x26, [sp, 8* 6]
	\pair_op x27, x28, [sp, 8* 8]
	\pair_op fp, lr,   [sp, 8*10]
.endm // .macro save_or_restore_on_stack

/// Retags a region.
///
/// void retag_region(void *base_with_tag, size_t size);
///
/// Both base and size must be multiples of 16.
.global mte_retag
mte_retag:
	and x2, x0, 0xf
	cbz x2, 2f
1:  // -> base is not 16-byte aligned
	brk 0xf1
	b 1b
2:  // -> base is 16-byte aligned
	and x2, x1, 0xf
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

// the rest are just random tests

//make_trampoline recurse
make_trampoline_with foo2 foo

.global foo
foo:
	// stg x0, [x0]
	// mov x0, 123
	add x0, x0, x1
	sub sp, sp, 16
	str x0, [sp, 0]
	ldr x0, [sp, 0]
	add sp, sp, 16
	ret // br x30

.macro make_trampoline_with_NaCl name unwrapped caller callee
.global \name
\name:
	trampoline_enter_NaCl \caller \callee
	mov x28, #0
	bl \unwrapped
	trampoline_exit_NaCl \caller \callee
	ret
.endm // .macro make_trampoline_with_NaCl


.macro trampoline_enter_NaCl caller callee
.extern RT_CALLEE_STACK
	// [x19][x20][x21][x22][x23][x24][x25][x26][x27][x28][fp/x29][original lr/x30]
	sub sp, sp, 8*12

	// save callee-saved registers and lr
	save_or_restore_on_stack stp

	// ====================================================
	// address space switch happens here (caller -> kernel)
	// ====================================================

	// save caller sp
	ldr x8, RT_CALLER_SAVED_SP_NEXT
	ldr x9, RT_CALLER_SAVED_SP_MAX
	ldr x10, =RT_CALLER_SAVED_SP_NEXT
	cmp x8, x9
	bls 2f

1:  // -> sp save space not available
	brk 0xf1
	b 1b

2:  // -> sp save space available
	mov x11, sp
	str x11, [x8], 8 // post-increment, store then add x8, x8, 8
	str x8, [x10]

	// wipe registers for confidentiality
	// domains are mutually untrusting
	//
	// we pass through the argument registers
	mov x8, 0
	mov x9, 0
	mov x10, 0
	mov x11, 0
	mov x12, 0
	mov x13, 0
	mov x14, 0
	mov x15, 0
	mov x16, 0
	mov x17, 0
	mov x18, 0

	mov x19, 0
	mov x20, 0
	mov x21, 0
	mov x22, 0
	mov x23, 0
	mov x24, 0
	mov x25, 0
	mov x26, 0
	mov x27, 0
	mov x28, 0

	mov fp, 0

	// ====================================================
	// address space switch happens here (kernel -> callee)
	// ====================================================

1:  // try to obtain new stack space
	adrp x8, RT_FREE_LIST_\callee // x8 is the top of the new stack
	ldr x8, [x8]
	cbnz x8, 3f


2:  // -> stack space not available
	brk 0xf2
	b 2b

3:  // -> stack space available
	add x9, x8, CALL_NEXT // x9 is .next, also the bottom of the new stack

	// try to remove from free list
	ldr x10, [x9] // read .next

	adrp x11, RT_FREE_LIST_\callee 

	mov x12, x8
	casa x12, x10, [x11] // expected, desired, ptr
	// cas sets expected to actual
	cmp x12, x8
	bne 1b // expected != actual

	// set new sp
	mov sp, x9

	// set .next, add to stack
	ldr x11, =RT_CALLEE_STACK
	ldr x10, [x11]
	str x10, [x9] // new_stack->next = previous_callee_stack
	str x8, [x11] // RT_CALLEE_STACK/callee = new_stack

	// we are ready to call the target

.endm // .macro trampoline_enter_NaCl


.macro trampoline_exit_NaCl caller callee

	ldr x11, =RT_CALLEE_STACK 
	ldr x10, [x11] // callee_stack = RT_CALLEE_STACK
	cbnz x10, 2f

1:  // -> RT_CALLEE_STACK  == 0
	brk 0xf3
	b 1b

2:  // -> RT_CALLEE_STACK  != 0
	add x13, x10, CALL_NEXT // callee_stack->next
	ldr x12, [x13] // previous_callee_stack = callee_stack->next
	str x12, [x11] // RT_CALLEE_STACK  = previous_callee_stack

1:  // try to reinsert to free list

	adrp x9, RT_FREE_LIST_\callee 

	//adr x8, RT_FREE_LIST_\callee
	ldr x8, [x9]

	str x8, [x13] // callee_stack->next = RT_FREE_LIST
	mov x11, x8
	casa x11, x10, [x9] // expected, desired, ptr
	// cas sets expected to actual
	cmp x11, x8
	bne 1b // expected != actual

	// ====================================================
	// address space switch happens here (callee -> kernel)
	// ====================================================

	// wipe registers for confidentiality
	// domains are mutually untrusting

	// we assume the function only returns one value
	mov x1, 0xbeef
	mov x2, 0xbeef
	mov x3, 0xbeef
	mov x4, 0xbeef
	mov x5, 0xbeef
	mov x6, 0xbeef
	mov x7, 0xbeef

	// preserve x8/xr (must be in shared heap to be accessible)
	mov x9, 0
	mov x10, 0
	mov x11, 0
	mov x12, 0
	mov x13, 0
	mov x14, 0
	mov x15, 0
	mov x16, 0
	mov x17, 0
	mov x18, 0

	mov x19, 0
	mov x20, 0
	mov x21, 0
	mov x22, 0
	mov x23, 0
	mov x24, 0
	mov x25, 0
	mov x26, 0
	mov x27, 0
	mov x28, 0

	mov fp, 0

	// no registers are trusted here except x0

	// restore caller sp
	ldr x8, RT_CALLER_SAVED_SP_NEXT
	ldr x9, =RT_CALLER_SAVED_SP_NEXT
	ldr x10, [x8, -8]! // pre-increment
	mov sp, x10
	str x8, [x9]

	// ====================================================
	// address space switch happens here (kernel -> caller)
	// ====================================================

	// restore callee-saved registers and lr
	save_or_restore_on_stack ldp

	add sp, sp, 8*12
.endm // .macro trampoline_exit_NaCl

make_trampoline_with_NaCl nf1 process_frames1 T A
make_trampoline_with_NaCl nf2 process_frames2 T B
make_trampoline_with_NaCl nf3 process_frames3 T C
make_trampoline_with_NaCl nf4 process_frames4 T D