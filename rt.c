// "Runtime"
//
// Each call through the trampoline is assigned a separate stack space with
// the register state before the callee preserved in a trusted region.

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "rt.h"
#include <sys/mman.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <sys/prctl.h>
size_t round_2_64(size_t input){
	return (input/64+1) * 64;
}
void Enable_MTE(){
	if (prctl(PR_SET_TAGGED_ADDR_CTRL,
			PR_TAGGED_ADDR_ENABLE | PR_MTE_TCF_SYNC | PR_MTE_TCF_ASYNC |
			(0xfffe << PR_MTE_TAG_SHIFT),
			0, 0, 0)) {
		perror("prctl() failed");
		abort();
	}
			printf("MTE enabled!\n");
}

void Enable_MTE_on_page(void* page_start){
	unsigned long page_sz = sysconf(_SC_PAGESIZE);
	if (mprotect(page_start, page_sz, PROT_READ | PROT_WRITE | PROT_MTE  )) {
		perror("mprotect() failed");
		abort();
	}
}
Call RT_CALLS[MAX_CALL_STACK] __attribute__ ((aligned (16)));

size_t RT_MESSAGE[6];
void * RT_FREE_LIST __attribute__ ((aligned (4096)))= 0;

void * RT_CALLEE_STACK = 0;

void * RT_CALLER_SAVED_SP[MAX_CALL_STACK];
void * RT_CALLER_SAVED_SP_NEXT = RT_CALLER_SAVED_SP;
void * RT_CALLER_SAVED_SP_MAX = RT_CALLER_SAVED_SP + MAX_CALL_STACK;

void RT_init() {
	for (size_t i = 0; i < MAX_CALL_STACK; ++i) {
		// set the sp to an conspicuous invalid value for easy debugging
		RT_CALLER_SAVED_SP[i] = (void*)0xfefe;

		Call *next;

		if (i == MAX_CALL_STACK - 1) {
			next = 0;
		} else {
			next = &RT_CALLS[i + 1];
		}

		RT_CALLS[i] = (Call) {
			.next = next,
		};
	}

	RT_FREE_LIST = &RT_CALLS[0];
}


#define Make_RT_init_(domain) \
Call * RT_CALLS_##domain; \
 \
size_t * RT_MESSAGE_##domain; \
void * RT_FREE_LIST_##domain __attribute__((aligned(4096))) = 0; \
 \
void * RT_CALLEE_STACK_##domain __attribute__ ( (section (".data_" #domain)) ) = 0; \
 \
 \
extern char _domain_##domain##_start[];\
extern char _domain_##domain##_end[];\
void RT_init_##domain()  { \
	RT_CALLS_##domain = mmap ( NULL, MAX_CALL_STACK * sizeof(Call), PROT_READ | PROT_WRITE| PROT_MTE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 ); \
	if( RT_CALLS_##domain == MAP_FAILED){ \
		perror("mprotect() failed"); \
		abort(); \
	} \
	mte_retag(RT_CALLS_##domain,round_2_64(MAX_CALL_STACK * sizeof(Call))); \ 
	RT_MESSAGE_##domain = mmap ( NULL, 6 * sizeof(size_t), PROT_READ | PROT_WRITE| PROT_MTE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 ); \
	if( RT_MESSAGE_##domain == MAP_FAILED){ \
		perror("mprotect() failed"); \
		abort(); \
	} \
	mte_retag(RT_MESSAGE_##domain,round_2_64(6 * sizeof(size_t))); \ 
	for (size_t i = 0; i < MAX_CALL_STACK; ++i) { \
 \
		Call *next; \
 \
		if (i == MAX_CALL_STACK - 1) { \
			next = 0; \
		} else { \
			next = &RT_CALLS_##domain[i + 1]; \
		} \
 \
		RT_CALLS_##domain[i] = (Call) { \
			.next = next, \
		}; \
	} \
 	RT_FREE_LIST_##domain = &RT_CALLS_##domain[0]; \
}\
//  	RT_FREE_LIST_##domain = &RT_CALLS_##domain[0]; \
//  printf("start :%p\nend :%p\n",_domain_##domain##_start,_domain_##domain##_end);\
//  void * page_st = _domain_##domain##_start;\
//  while( page_st != _domain_##domain##_end){\
//  printf("Enable_MTE_on_page: %p\n",page_st);\
//  printf("page_st[0]: %d\n",*(char*)page_st);\
// 	Enable_MTE_on_page(page_st);\
// 	page_st = (void*) (((size_t)page_st)+4096);\
//  }\
//  mte_retag(_domain_##domain##_start, _domain_##domain##_end - _domain_##domain##_start);\
// } \
 
Make_RT_init_(A)
Make_RT_init_(B)
Make_RT_init_(C)
Make_RT_init_(D)
// Make_RT_init_(E)
// Make_RT_init_(F)
// Make_RT_init_(G)
// Make_RT_init_(H)
// Make_RT_init_(I)
// Make_RT_init_(J)
// Make_RT_init_(K)

void MTE_init() {
	//Enable_MTE();
	 RT_init_A();
	 RT_init_B();
	 RT_init_C();
	 RT_init_D();
	// RT_init_E();
	// RT_init_F();
	// RT_init_G();
	// RT_init_H();
	// RT_init_I();
	// RT_init_J();
	// RT_init_K();
	printf("MTE init done\n");
}

void * domain_alloc(size_t sz, int domain){
	sz = round_2_64(sz);
	void * ret = aligned_alloc(4096, sz);
	void * tmp = ret;
	size_t counter = 0;
	while(sz > counter){
		//printf("Enable_MTE_on_page %p\n",tmp);
		Enable_MTE_on_page(tmp);
		tmp = (size_t)tmp + 4096;
		counter+=4096;
	}
	mte_retag(ret,sz);
	return ret;
}