.text

// Trampoline with context
//
// This is built as a flat binary, with context appended when loaded
//
// [Trampoline Code][Destination][Caller Base][Callee Base]

// Configuration:
//
// - CALLEE_IS_TCB

// x0-x8: arguments and return value
// x9-x15: temporary registers
// x19-x29: callee-saved
// x29/fp: frame pointer
// x30/lr: link register

.macro get_destination dst, ctx
	ldr \dst, [\ctx, 0]
.endm

.macro get_caller_base dst, ctx
	ldr \dst, [\ctx, 8]
.endm

.macro get_callee_base dst, ctx
	ldr \dst, [\ctx, 16]
.endm

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

adr x9, ctx

get_destination x10, x9
get_caller_base x11, x9
get_callee_base x12, x9

#ifdef CALLEE_IS_TCB

// Only save X30/LR
sub sp, sp, 16
str x30, [sp]

#else

// Save callee-saved registers
sub sp, sp, 8*12
save_or_restore_on_stack stp

#endif

// per-thread stack offset
//mrs c13, cid_el0
//lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
//add x13, x13, 0x10000 // add STACK_OFFSET

mov x13, 0x10000 // TODO

// get caller SP save location
add x14, x11, x13 // x14 = caller_base + x13

// save caller saved SP
sub sp, sp, 16
ldr x15, [x14]
str x15, [sp]

// save caller SP
mov x15, sp
str x15, [x14]

// switch to callee SP
add x14, x12, x13 // c14 = callee_base + x13

ldr x15, [x14]
mov sp, x15

//brk 0

#ifndef CALLEE_IS_TCB
// wipe

mov x8, 0
mov x9, 0
#mov x10, 0
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
#endif

// call
blr x10

// we are back!

adr x9, ctx

get_destination x10, x9
get_caller_base x11, x9
get_callee_base x12, x9

// per-thread stack offset
//mrs c13, cid_el0
//lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
//add x13, x13, 0x10000 // add STACK_OFFSET

mov x13, 0x10000 // TODO

// get caller SP save location
add x14, x11, x13 // c14 = caller_base + x13

// restore caller SP
ldr x15, [x14]
mov sp, x15

// restore previous caller saved SP
ldr x15, [sp]
str x15, [x14]
add sp, sp, 16

#ifdef CALLEE_IS_TCB

// Restore C30/LR
ldr x30, [sp]
add sp, sp, 16

#else

// Restore all callee-saved registers
save_or_restore_on_stack ldp
add sp, sp, 8*12

#endif

ret

.align 8
ctx:
