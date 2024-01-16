// Trampoline with context
//
// This is built as a flat binary, with context appended when loaded
//
// [Trampoline Code][Destination][Caller Base][Callee Base]

// Configuration:
//
// - CALLEE_IS_TCB
// - ENABLE_FPU

.intel_syntax noprefix

.macro get_destination dst
	mov \dst, [rip + ctx]
.endm

.macro get_caller_base dst
	mov \dst, [rip + ctx + 8]
.endm

.macro get_callee_base dst
	mov \dst, [rip + ctx + 16]
.endm

// rax, r10, r11

#ifndef CALLEE_IS_TCB

// save and wipe callee-saved registers
push r12
push r13
push r14
push r15
push rbx
push rbp

xor rax, rax
xor rbx, rbx
xor rbp, rbp
xor r10, r10
xor r11, r11
xor r12, r12
xor r13, r13
xor r14, r14
xor r15, r15

#endif

// get caller stack base
get_caller_base r10
add r10, 0x10000 // TODO: per-thread stack offset

// save caller saved RSP
mov r11, [r10]
push r11

// save caller RSP
mov [r10], rsp

// get callee stack base
get_callee_base r10
add r10, 0x10000 // TODO: per-thread stack offset

// switch to callee RSP
mov rsp, [r10]

// ...
get_destination rax
call rax
// ...

// get caller stack base
get_caller_base r10
add r10, 0x10000 // TODO: per-thread stack offset

// restore caller RSP
mov rsp, [r10]

// restore caller saved RSP
pop r11
mov [r10], r11

#ifndef CALLEE_IS_TCB

// restore callee-saved registers
pop rbp
pop rbx
pop r15
pop r14
pop r13
pop r12

#endif

ret

.align 16
ctx:
