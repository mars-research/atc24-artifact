; Trampoline

default rel

%define CALL_SIZE (2*1024*1024+8)
%define CALL_END_OF_STACK (2*1024*1024)
%define CALL_NEXT (2*1024*1024)
%define MAX_CALL_STACK 20

%ifdef USE_FXSAVE
%error "USE_FXSAVE has been renamed to USE_SIMD"
%endif

; Thread- and domain-local variables
extern RT_CALLER_SAVED_RSP      ; local to caller
extern RT_CALLER_SAVED_RSP_NEXT ; local to caller
extern RT_CALLER_SAVED_RSP_MAX  ; local to caller
extern RT_CALLEE_STACK          ; local to callee
; Thread-local variables
; extern RT_MESSAGE

; Global variables
extern RT_FREE_LIST

extern RT_error

; Save the state to trusted storage.
%macro trampoline_enter 0
	; [r15][r14][r13][r12][rbx][rbp][original rsp][fxsave and padding]

%ifdef USE_SIMD
	; fxsave target must be 16-byte aligned
	mov rax, rsp
	sub rsp, 512
	and rsp, -16
	fxsave [rsp]
	push rax
%endif

	; save 7 callee-saved
	push rbp
	push rbx
	push r12
	push r13
	push r14
	push r15
	pushfq

	; RT_MESSAGE is a shared memory location
	;mov rbp, RT_MESSAGE
	;mov [rbp + 8*0], rdi
	;mov [rbp + 8*1], rsi
	;mov [rbp + 8*2], rdx
	;mov [rbp + 8*3], rcx
	;mov [rbp + 8*4], r8
	;mov [rbp + 8*5], r9

	; ====================================================
	; address space switch happens here (caller -> kernel)
	; ====================================================

	; save caller rsp
	mov r10, [RT_CALLER_SAVED_RSP_NEXT]
	cmp r10, [RT_CALLER_SAVED_RSP_MAX]
	jb .rsp_space_available

.rsp_space_not_available:
	; "Fatal: No RSP save space available"
	mov rdi, 2
	jmp RT_error
.rsp_space_available:
	mov [r10], rsp
	add r10, 8
	mov [RT_CALLER_SAVED_RSP_NEXT], r10

%ifdef USE_REGWIPE
	; wipe registers for confidentiality
	; domains are mutually untrusting
	;
	; we pass through the argument registers
	xor rax, rax
	xor rbx, rbx
	xor rbp, rbp
	xor rsp, rsp
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
%ifdef USE_SIMD
	vzeroall
%endif
%endif

	; ====================================================
	; address space switch happens here (kernel -> callee)
	; ====================================================

	; mov rbp, RT_MESSAGE
	; mov rdi, [rbp + 8*0]
	; mov rsi, [rbp + 8*1]
	; mov rdx, [rbp + 8*2]
	; mov rcx, [rbp + 8*3]
	; mov r8, [rbp + 8*4]
	; mov r9, [rbp + 8*5]

.try_obtain_stack_space:
	mov rax, [RT_FREE_LIST]
	cmp rax, 0
	jne .stack_available

.stack_not_available:
	int3
	jmp .stack_not_available
.stack_available:
	mov r10, rax
	add r10, CALL_NEXT ; also .rsp

	; try to remove from free list
	mov r11, [r10] ; .next
%ifdef USE_ATOMIC
	lock cmpxchg [RT_FREE_LIST], r11
	jnz .try_obtain_stack_space
%else
	mov [RT_FREE_LIST], r11
%endif

	; set new rsp - now we have rsp
	mov rsp, r10

	; set .next, add to stack
	mov r11, [RT_CALLEE_STACK]
	mov [r10], r11
	mov [RT_CALLEE_STACK], rax

	; we are ready to call target
%endmacro

; Restore the state from trusted storage.
%macro trampoline_exit 0
.trampoline_exit:
	; final clean up before passing control back to caller
	; we need to return the stack space to the free list

	mov r10, [RT_CALLEE_STACK]
	cmp r10, 0
	jne .valid_return
.invalid_return:
	; "Fatal: No call in progress"
	mov rdi, 1
	jmp RT_error
	int3
.valid_return:

	; pop stack
	mov rdi, [r10 + CALL_NEXT]
	mov [RT_CALLEE_STACK], rdi

	; set .next, reinsert to free list
	mov rdi, rax
.try_return_stack_space:
	mov rax, [RT_FREE_LIST]
	mov [r10 + CALL_NEXT], rax
%ifdef USE_ATOMIC
	lock cmpxchg [RT_FREE_LIST], r10
	jnz .try_return_stack_space
%else
	mov [RT_FREE_LIST], r10
%endif

	mov rax, rdi

	; ====================================================
	; address space switch happens here (callee -> kernel)
	; ====================================================

	; wipe registers for confidentiality
	; domains are mutually untrusting
%ifdef USE_REGWIPE
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rbp, rbp
	xor rsp, rsp
	xor rdi, rdi
	xor rsi, rsi
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
%ifdef USE_SIMD
	vzeroall
%endif
%endif

	; no registers are trusted here except for rax

	; restore caller rsp
	mov r10, [RT_CALLER_SAVED_RSP_NEXT]
	sub r10, 8
	mov rsp, [r10]
	mov [RT_CALLER_SAVED_RSP_NEXT], r10

	; ====================================================
	; address space switch happens here (kernel -> caller)
	; ====================================================

	popfq
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp

%ifdef USE_SIMD
	pop rdi
	fxrstor [rsp]
	mov rsp, rdi
%endif

	; we are at original rsp
	; we are ready to return to the real caller
	ret
%endmacro

%macro make_trampoline_with 2
extern %2
global %1
%ifdef USE_TRAMPOLINE
%1:
	trampoline_enter
	call %2
	trampoline_exit
%else
%1:
	jmp %2
%endif
%endmacro

%macro make_trampoline 1
make_trampoline_with %1, _%1
%endmacro

; Save the state to trusted storage.
%macro trampoline_enter_SFI 2
extern CURRENT_DOM
extern DOM_%1
extern DOM_%2
extern RT_FREE_LIST_%1
extern RT_FREE_LIST_%2
extern RT_CALLEE_STACK_%1
extern RT_CALLEE_STACK_%2
	; [r15][r14][r13][r12][rbx][rbp][original rsp][fxsave and padding]
	; argument register rdi rsi rdx

%ifdef USE_CHECK_CALLER
	mov r10, [DOM_%1]
	mov r11, [CURRENT_DOM]
	cmp r11, r10
	je .caller_vaild

.caller_not_vaild:
	; "Caller is not vaild"
	mov rdi, 4
	jmp RT_error
%endif
.caller_vaild:


%ifdef USE_SIMD
	; fxsave target must be 16-byte aligned
	mov rax, rsp
	sub rsp, 512
	and rsp, -16
	fxsave [rsp]
	push rax
%endif

	; save 7 callee-saved
	push rbp
	push rbx
	push r12
	push r13
	push r14
	push r15
	pushfq

	

	; RT_MESSAGE is a shared memory location
	;mov rbp, RT_MESSAGE
	;mov [rbp + 8*0], rdi
	;mov [rbp + 8*1], rsi
	;mov [rbp + 8*2], rdx
	;mov [rbp + 8*3], rcx
	;mov [rbp + 8*4], r8
	;mov [rbp + 8*5], r9

	; ====================================================
	; address space switch happens here (caller -> kernel)
	; ====================================================

	; save caller rsp
	mov r10, [RT_CALLER_SAVED_RSP_NEXT]
	cmp r10, [RT_CALLER_SAVED_RSP_MAX]
	jb .rsp_space_available

.rsp_space_not_available:
	; "Fatal: No RSP save space available"
	mov rdi, 2
	jmp RT_error
.rsp_space_available:
	mov [r10], rsp
	add r10, 32
	mov [RT_CALLER_SAVED_RSP_NEXT], r10
	mov r11, [DOM_%1]
	mov [r10 - 24], r11
	mov r11, [DOM_%2]
	mov [r10 - 16], r11
	mov [r10 - 8], rcx

%ifdef USE_REGWIPE
	; wipe registers for confidentiality
	; domains are mutually untrusting
	;
	; we pass through the argument registers
	xor rax, rax
	xor rbx, rbx
	xor rbp, rbp
	xor rsp, rsp
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
%ifdef USE_SIMD
	vzeroall
%endif
%endif

	; ====================================================
	; address space switch happens here (kernel -> callee)
	; ====================================================

	; mov rbp, RT_MESSAGE
	; mov rdi, [rbp + 8*0]
	; mov rsi, [rbp + 8*1]
	; mov rdx, [rbp + 8*2]
	; mov rcx, [rbp + 8*3]
	; mov r8, [rbp + 8*4]
	; mov r9, [rbp + 8*5]

.try_obtain_stack_space:
	mov rax, [RT_FREE_LIST_%2]
	cmp rax, 0
	jne .stack_available

.stack_not_available:
	mov rdi, 1
	jmp RT_error
	int3
	jmp .stack_not_available
.stack_available:
	mov r10, rax
	add r10, CALL_NEXT ; also .rsp

	; try to remove from free list
	mov r11, [r10] ; .next
%ifdef USE_ATOMIC
	lock cmpxchg [RT_FREE_LIST_%2], r11
	jnz .try_obtain_stack_space
%else
	mov [RT_FREE_LIST_%2], r11
%endif

	; set new rsp - now we have rsp
	mov rsp, r10

	; set .next, add to stack
	mov r11, [RT_CALLEE_STACK_%2]
	mov [r10], r11
	mov [RT_CALLEE_STACK_%2], rax

	; we are ready to call target
%endmacro

%macro trampoline_exit_SFI 2
	; final clean up before passing control back to caller
	; we need to return the stack space to the free list

	mov r10, [RT_CALLEE_STACK_%2]
	cmp r10, 0
	jne .valid_return
.invalid_return:
	; "Fatal: No call in progress"
	mov rdi, 1
	jmp RT_error
	int3
.valid_return:

	; pop stack
	mov rdi, [r10 + CALL_NEXT]
	mov [RT_CALLEE_STACK_%2], rdi

	; set .next, reinsert to free list
	mov rdi, rax
.try_return_stack_space:
	mov rax, [RT_FREE_LIST_%2]
	mov [r10 + CALL_NEXT], rax
%ifdef USE_ATOMIC
	lock cmpxchg [RT_FREE_LIST_%2], r10
	jnz .try_return_stack_space
%else
	mov [RT_FREE_LIST_%2], r10
%endif

	mov rax, rdi

	; ====================================================
	; address space switch happens here (callee -> kernel)
	; ====================================================

	; wipe registers for confidentiality
	; domains are mutually untrusting
%ifdef USE_REGWIPE
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rbp, rbp
	xor rsp, rsp
	xor rdi, rdi
	xor rsi, rsi
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
%ifdef USE_SIMD
	vzeroall
%endif
%endif

	; no registers are trusted here except for rax

	; restore caller rsp
	mov r10, [RT_CALLER_SAVED_RSP_NEXT]
	sub r10, 32
	mov rsp, [r10]
	mov [RT_CALLER_SAVED_RSP_NEXT], r10

	mov r11, [r10 + 8]
	cmp r11, [DOM_%1]
	je .caller_id_vaild

.caller_id_not_vaild:
	mov rdi, 6
	jmp RT_error
.caller_id_vaild:
	mov r11, [r10 + 16]
	cmp r11, [DOM_%2]
	je .callee_id_vaild

.callee_id_not_vaild:
	mov rdi, 6
	jmp RT_error
.callee_id_vaild:

	mov rcx, [r10 + 24]
	; ====================================================
	; address space switch happens here (kernel -> caller)
	; ====================================================


	popfq
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp

%ifdef USE_SIMD
	pop rdi
	fxrstor [rsp]
	mov rsp, rdi
%endif






%ifdef USE_CHECK_CALLER
	mov r11, [DOM_%1]
	mov r10, [CURRENT_DOM]
	cmp r10, r11
	je .exit_caller_vaild

.exit_caller_not_vaild:

	mov rdi, 8
	jmp RT_error
%endif
.exit_caller_vaild:
	; we are at original rsp
	; we are ready to return to the real caller
%endmacro



%macro make_trampoline_SFI_loader_with 4
extern %2
extern CURRENT_DOM
extern DOM_%3
extern DOM_%4
extern R15_%4
extern RT_FREE_LIST_%3
extern RT_FREE_LIST_%4
extern RT_CALLEE_STACK_%3
extern RT_CALLEE_STACK_%4
global %1
%ifdef USE_TRAMPOLINE
%1:

	trampoline_enter_SFI %3, %4

%ifdef USE_CHECK_CALLER
	mov r10, [DOM_%4]
	mov [CURRENT_DOM], r10
%endif

	mov r15, [R15_%4]
	call [%2]

	;; Exit callee domain

%ifdef USE_CHECK_CALLER
	mov r10, [DOM_%3]
	mov [CURRENT_DOM], r10
%endif 

	trampoline_exit_SFI %3, %4

	ret
%else
%1:
	jmp [%2]
%endif
%endmacro

%macro make_trampoline_SFI_loader 3
make_trampoline_SFI_loader_with %1, _%1, %2, %3
%endmacro