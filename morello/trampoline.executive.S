#ifdef __CHERI_PURE_CAPABILITY__
#error Must be compiled for Hybrid Capability mode (PSTATE.C64=0)
#endif

.text

// DEBUG
//#define CALLEE_IS_TCB

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

// We assume that we are on the TCB stack

.macro get_super_cap dst, ctx
	ldr \dst, [\ctx, 0]
.endm

.macro get_caller_cap dst, ctx
	ldr \dst, [\ctx, 16]
.endm

.macro get_callee_cap dst, ctx
	ldr \dst, [\ctx, 32]
.endm

adr x9, ctx

get_super_cap c10, x9
get_caller_cap c11, x9
get_callee_cap c12, x9

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

// BREAKDOWN -> 19 cycles
// cap branch + cap ret = 11 cycles (known)
// mov = 1 cycle (known)
// load caps + save registers = 7 cycles

//add sp, sp, 16*12
//mov x0, 0
//retr c30

#sub sp, sp, 16
#str c30, [sp]

// per-thread stack offset
//mrs c13, cid_el0
//lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
//add x13, x13, 0x10000 // add STACK_OFFSET

mov x13, 0x10000 // DEBUG

// [caller sp 1][stack 1][sp save 2][...]

// A*-> B -> A*-> B

// get caller RSP save location
add c14, c11, x13 // x14 = caller_cap + x13

// save caller saved RSP
sub sp, sp, 16
ldr x15, [c14]
str x15, [sp]

// save caller RSP
mrs x8, rsp_el0 // BREAKDOWN
mrs x15, rsp_el0
str x15, [c14]

// switch to callee RSP
add c14, c12, x13 // c14 = callee_cap + x13

ldr x15, [c14] // load with callee
msr rsp_el0, x15

// BREAKDOWN -> 21 cycles
// save + switch stack = 2

//msr rsp_el0, x8
//add sp, sp, 16*12 + 16
//mov x0, 0
//retr c30

// switch to callee RDDC
msr rddc_el0, c12

// prepare target
mov x9, x10
// seal c30, c12, rb // the value of callee_cap is at the rets trampoline
seal c7, c12, rb // BREAKDOWN

// BREAKDOWN -> 22 cycles
// switch rddc + seal = ~1

//msr rsp_el0, x8
//add sp, sp, 16*12 + 16
//mov x0, 0
//retr c30

#ifndef CALLEE_IS_TCB
// wipe

////mov x8, 0
nop // BREAKDOWN

//mov x9, 0
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

// BREAKDOWN -> 39 cycles (with restore) (unstable)
// wipe + restore = 16

//msr rsp_el0, x8
//add sp, sp, 16
//ldr c19, [sp, 16 * 0]
//ldr c20, [sp, 16 * 1]
//ldr c21, [sp, 16 * 2]
//ldr c22, [sp, 16 * 3]
//ldr c23, [sp, 16 * 4]
//ldr c24, [sp, 16 * 5]
//ldr c25, [sp, 16 * 6]
//ldr c26, [sp, 16 * 7]
//ldr c27, [sp, 16 * 8]
//ldr c28, [sp, 16 * 9]
//ldr c29, [sp, 16 *10]
//ldr c30, [sp, 16 *11]
//add sp, sp, 16*12
//mov x0, 0
//retr c30

// call
//blrr c30 // destroyed and replaced with return cap
blrr c7 // BREAKDOWN

#mov x0, sp

#ldr w0, [x30]
#mov x0, x30

#brk 0

// we are back!

adr x9, ctx

get_super_cap c10, x9
get_caller_cap c11, x9
get_callee_cap c12, x9

// per-thread stack offset
//mrs c13, cid_el0
//lsl x13, x13, 22 // x13 = CID_EL0.value * 4MiB (STACK_SIZE)
//add x13, x13, 0x10000 // add STACK_OFFSET

mov x13, 0x10000

// save callee RSP
add c14, c12, x13 // c14 = callee_cap + x13

mrs x15, rsp_el0
str x15, [c14]

// switch to caller RDDC
msr rddc_el0, c11

// get caller RSP save location
add c14, c11, x13 // c14 = caller_cap + x13

// restore caller RSP
ldr x15, [c14]
msr rsp_el0, x15

// restore previous caller saved RSP
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

retr c30

.align 8
ctx:
