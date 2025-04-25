// "Runtime"
//
// Each call through the trampoline is assigned a separate stack space with
// the register state before the callee preserved in a trusted region.
#define _GNU_SOURCE  
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "rt.h"
#include <dlfcn.h>
#include <link.h>
#include <errno.h>
#include "include.h"

extern char _trampoline_start[];
extern char _trampoline_end[];
extern char _trampoline_data_start[];
extern char _trampoline_data_end[];
extern char _ro_trampoline_data_start[];
extern char _ro_trampoline_data_end[];
Call RT_CALLS[MAX_CALL_STACK];

size_t RT_MESSAGE[6];
void * RT_FREE_LIST = 0;

void * RT_CALLEE_STACK __attribute__ ( (section (".trampoline_data")) ) = 0;

RSP RT_CALLER_SAVED_RSP[MAX_CALL_STACK]__attribute__ ( (section (".trampoline_data")) );
void * RT_CALLER_SAVED_RSP_NEXT __attribute__ ((section (".trampoline_data"))) = RT_CALLER_SAVED_RSP;
void * RT_CALLER_SAVED_RSP_MAX __attribute__ ((section (".trampoline_data"))) = RT_CALLER_SAVED_RSP + MAX_CALL_STACK * 4;

domain_id CURRENT_DOM __attribute__ ( (section (".ro_trampoline_data")) ) = 0;
domain_id DOM_T __attribute__ ( (section (".ro_trampoline_data")) ) = 0;
domain_id DOM_A __attribute__ ( (section (".ro_trampoline_data")) ) = 1;
domain_id DOM_B __attribute__ ( (section (".ro_trampoline_data")) ) = 2;
domain_id DOM_C __attribute__ ( (section (".ro_trampoline_data")) ) = 3;
domain_id DOM_D __attribute__ ( (section (".ro_trampoline_data")) ) = 4;
domain_id DOM_E __attribute__ ( (section (".ro_trampoline_data")) ) = 5;
domain_id DOM_F __attribute__ ( (section (".ro_trampoline_data")) ) = 6;
domain_id DOM_G __attribute__ ( (section (".ro_trampoline_data")) ) = 7;
domain_id DOM_H __attribute__ ( (section (".ro_trampoline_data")) ) = 8;
domain_id DOM_I __attribute__ ( (section (".ro_trampoline_data")) ) = 9;
domain_id DOM_J __attribute__ ( (section (".ro_trampoline_data")) ) = 10;
domain_id DOM_K __attribute__ ( (section (".ro_trampoline_data")) ) = 11;
domain_id DOM_L __attribute__ ( (section (".ro_trampoline_data")) ) = 12;
domain_id DOM_M __attribute__ ( (section (".ro_trampoline_data")) ) = 13;


void RT_error(size_t error) {
	switch (error) {
		case 0:
			fprintf(stderr,"Fatalro_: No stack space available.\n");
			break;

		case 1:
			fprintf(stderr,"Fatal: No call in process.\n");
			break;

		case 2:
			fprintf(stderr,"Fatal: No RSP save space available.\n");
			break;
		
		case 3:
			fprintf(stderr,"Fatal: Fake rax value\n");
			break;

		case 4:
			fprintf(stderr,"Fatal: Caller is not vaild\n");
			break;

		case 5:
			fprintf(stderr,"Fatal: Invaild_buffer_ownership\n");
			break;

		case 6:
			fprintf(stderr,"Fatal: Fake return!\n");
			break;

		case 7:
			fprintf(stderr,"Fatal: Stack Not Available!\n");
			break;

		case 8:
			fprintf(stderr,"Fatal: Exit path Caller is not vaild!\n");
			break;
	}
	exit(EXIT_FAILURE);
}

void RT_init() {
	for (size_t i = 0; i < MAX_CALL_STACK; ++i) {
		Call *next;

		if (i == MAX_CALL_STACK - 1) {
			next = 0;
		} else {
			next = &RT_CALLS[i + 1];
		}

		RT_CALLS[i].next = next;
	}

	RT_FREE_LIST = &RT_CALLS[0];
}

void * R15_A;
void * PRIVATE_BUFFER_A;
int SHARED_BUFFER_A[BUFFER_SIZE] __attribute__((aligned(4096)));
extern char _domain_a_start[];
extern char _domain_a_end[];

Call *RT_CALLS_A __attribute__ ((section (".domain_a")));

size_t RT_MESSAGE_A[6] __attribute__ ((section (".domain_a")));
void * RT_FREE_LIST_A __attribute__ ((section (".domain_a"))) = 0;

void * RT_CALLEE_STACK_A __attribute__ ((section (".domain_a"))) = 0;


void * RT_CALLER_SAVED_RSP_A[MAX_CALL_STACK] __attribute__ ((section (".domain_a"))) ;
void * RT_CALLER_SAVED_RSP_NEXT_A __attribute__ ((section (".domain_a"))) = RT_CALLER_SAVED_RSP_A;
void * RT_CALLER_SAVED_RSP_MAX_A __attribute__ ((section (".domain_a"))) = RT_CALLER_SAVED_RSP_A + MAX_CALL_STACK;


void RT_init_A() {
}

void * R15_B;
void * PRIVATE_BUFFER_B;
int SHARED_BUFFER_B[BUFFER_SIZE] __attribute__((aligned(4096)));
extern char _domain_b_start[];
extern char _domain_b_end[];

Call *RT_CALLS_B __attribute__ ((section (".domain_b")));

size_t RT_MESSAGE_B[6] __attribute__ ((section (".domain_b")));
void * RT_FREE_LIST_B __attribute__ ((section (".domain_b")))= 0;

void * RT_CALLEE_STACK_B __attribute__ ((section (".domain_b")))= 0;

void * RT_CALLER_SAVED_RSP_B[MAX_CALL_STACK]__attribute__ ((section (".domain_b")));
void * RT_CALLER_SAVED_RSP_NEXT_B __attribute__ ((section (".domain_b")))= RT_CALLER_SAVED_RSP_B;
void * RT_CALLER_SAVED_RSP_MAX_B __attribute__ ((section (".domain_b")))= RT_CALLER_SAVED_RSP_B + MAX_CALL_STACK;


void RT_init_B() {
}

Call *RT_CALLS_T;

size_t RT_MESSAGE_T[6]; 
void * RT_FREE_LIST_T = 0;

void * RT_CALLEE_STACK_T; 

void * RT_CALLER_SAVED_RSP_T[MAX_CALL_STACK];
void * RT_CALLER_SAVED_RSP_NEXT_T;
void * RT_CALLER_SAVED_RSP_MAX_T;
void Stack_init_T() {
    RT_CALLS_T = malloc(MAX_CALL_STACK * sizeof(Call));
    memset(RT_CALLS_T, 0, MAX_CALL_STACK * sizeof(Call));
	for (size_t i = 0; i < MAX_CALL_STACK; ++i) { 
		Call *next; 
		if (i == MAX_CALL_STACK - 1) { 
			next = 0; 
		} else { 
			next = &RT_CALLS_T[i + 1]; 
		} 
		RT_CALLS_T[i].next = next; 
	} 
	RT_FREE_LIST_T = &RT_CALLS_T[0]; 
}

#define Make_RT_init_(domain, _domain) \
	extern char _domain_##_domain##_start[]; \
	extern char _domain_##_domain##_end[]; \
	Call * RT_CALLS_##domain __attribute__ ((section (".domain_" #domain)))=0;\
	size_t RT_MESSAGE_##domain[6] __attribute__ ((section (".domain_" #domain))); \
	void * RT_FREE_LIST_##domain __attribute__ ((section (".domain_" #domain))) = 0; \
	void * RT_CALLEE_STACK_##domain __attribute__ ((section (".domain_" #domain))) = 0; \
	void * RT_CALLER_SAVED_RSP_##domain[MAX_CALL_STACK]; \
	void * RT_CALLER_SAVED_RSP_NEXT_##domain = RT_CALLER_SAVED_RSP_##domain; \
	void * RT_CALLER_SAVED_RSP_MAX_##domain = RT_CALLER_SAVED_RSP_##domain + MAX_CALL_STACK; \
	void * PRIVATE_BUFFER_##domain; \
	void * R15_##domain = 0;\
	int SHARED_BUFFER_##domain[BUFFER_SIZE] __attribute__((aligned(4096))); \
	void RT_init_##domain() {	\
} \


#define Make_Stack_init_(domain) \
    void Stack_init_##domain() { \
    RT_CALLS_##domain = malloc(MAX_CALL_STACK * sizeof(Call)); \
    memset(RT_CALLS_##domain, 0, MAX_CALL_STACK * sizeof(Call)); \
	for (size_t i = 0; i < MAX_CALL_STACK; ++i) { \
		Call *next; \
		if (i == MAX_CALL_STACK - 1) { \
			next = 0; \
		} else { \
			next = &RT_CALLS_##domain[i + 1]; \
		} \
		RT_CALLS_##domain[i].next = next; \
	} \
	RT_FREE_LIST_##domain = &RT_CALLS_##domain[0]; \
}\

Make_RT_init_(C,c)
Make_RT_init_(D,d)
Make_RT_init_(E,e)
Make_RT_init_(F,f)
Make_RT_init_(G,g)
Make_RT_init_(H,h)
Make_RT_init_(I,i)
Make_RT_init_(J,j)
Make_RT_init_(K,k)
Make_RT_init_(L,l)
Make_RT_init_(M,m)

Make_Stack_init_(A);
Make_Stack_init_(B);
Make_Stack_init_(C);
Make_Stack_init_(D);
Make_Stack_init_(E);
Make_Stack_init_(F);
Make_Stack_init_(G);
Make_Stack_init_(H);
Make_Stack_init_(I);
Make_Stack_init_(J);
Make_Stack_init_(K);
Make_Stack_init_(L);
Make_Stack_init_(M);


void MPK_init(){
	RT_init_A();
	RT_init_B();
	RT_init_C();
	RT_init_D();
	RT_init_E();
	RT_init_F();
	RT_init_G();
	RT_init_H();
	RT_init_I();
	RT_init_J();
	RT_init_K();
	RT_init_L();
	RT_init_M();

    Stack_init_A();
    Stack_init_B();
    Stack_init_C();
    Stack_init_D();
    // Stack_init_E();
    // Stack_init_F();
    // Stack_init_G();
    // Stack_init_H();
    // Stack_init_I();
    // Stack_init_J();
    // Stack_init_K();
    // Stack_init_L();
    // Stack_init_M();
    // Stack_init_T();
}






