#ifndef __CHERI_PURE_CAPABILITY__
#error Must be compiled for Pure Capability mode (PSTATE.C64=1)
#endif

.text

// Trampoline with context
//
// This is built as a flat binary, with context appended when loaded
//
// [Trampoline Code][Super Cap][Caller Cap][Callee Cap]

// Configuration:
//
// - CALLEE_IS_TCB

// x0-x8: arguments and return value
// x9-x15: temporary registers
// x19-x29: callee-saved
// x29/fp: frame pointer
// x30/lr: link register

.macro get_super_cap dst, ctx
	ldr \dst, [\ctx, 0]
.endm

.macro get_caller_cap dst, ctx
	ldr \dst, [\ctx, 16]
.endm

.macro get_callee_cap dst, ctx
	ldr \dst, [\ctx, 32]
.endm

// There is no way to derive from PCC in Hybrid mode?
adr c9, ctx

get_super_cap c10, c9
get_caller_cap c11, c9
get_callee_cap c12, c9

#ifdef CALLEE_IS_TCB

// Only save C30/LR
sub sp, sp, 16
str c30, [sp]

#else

// Save callee-saved registers
sub sp, sp, 16*12
str c19, [sp, 16 * 0]
str c20, [sp, 16 * 1]
str c21, [sp, 16 * 2]
str c22, [sp, 16 * 3]
str c23, [sp, 16 * 4]
str c24, [sp, 16 * 5]
str c25, [sp, 16 * 6]
str c26, [sp, 16 * 7]
str c27, [sp, 16 * 8]
str c28, [sp, 16 * 9]
str c29, [sp, 16 *10]
str c30, [sp, 16 *11]

#endif

// per-thread stack offset
mrs c13, cid_el0
lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
add x13, x13, 0x10000 // add STACK_OFFSET

// get caller SP save location
add c14, c11, x13 // x14 = caller_cap + x13

// save caller saved SP
sub sp, sp, 16
ldr x15, [c14]
str x15, [sp]

// save caller SP
mov x15, sp
str x15, [c14]

#brk 0

// switch to callee SP
add c14, c12, x13 // c14 = callee_cap + x13

ldr x15, [c14] // load with callee
mov sp, x15

// switch to callee DDC
msr ddc, c12

//brk 0

// prepare target
mov x9, x10
seal c30, c12, rb // the value of callee_cap is at the rets trampoline

#ifndef CALLEE_IS_TCB
// wipe

mov x8, 0
#mov x9, 0
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
#endif

// call
blr c30 // destroyed and replaced with return cap

#mov x0, sp

#ldr w0, [x30]
#mov x0, x30

#brk 0

// we are back!

adr c9, ctx

get_super_cap c10, c9
get_caller_cap c11, c9
get_callee_cap c12, c9

// per-thread stack offset
mrs c13, cid_el0
lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
add x13, x13, 0x10000 // add STACK_OFFSET
#mov x0, x13

// switch to caller DDC
msr ddc, c11

// get caller SP save location
add c14, c11, x13 // c14 = caller_cap + x13

// restore caller SP
ldr x15, [c14]
mov sp, x15

// restore previous caller saved SP
ldr x15, [sp]
str x15, [c14]
add sp, sp, 16

#ifdef CALLEE_IS_TCB

// Restore C30/LR
ldr c30, [sp]
add sp, sp, 16

#else

// Restore all callee-saved registers
ldr c19, [sp, 16 * 0]
ldr c20, [sp, 16 * 1]
ldr c21, [sp, 16 * 2]
ldr c22, [sp, 16 * 3]
ldr c23, [sp, 16 * 4]
ldr c24, [sp, 16 * 5]
ldr c25, [sp, 16 * 6]
ldr c26, [sp, 16 * 7]
ldr c27, [sp, 16 * 8]
ldr c28, [sp, 16 * 9]
ldr c29, [sp, 16 *10]
ldr c30, [sp, 16 *11]
add sp, sp, 16*12

#endif

ret // capability return because C64

.align 8
ctx:
