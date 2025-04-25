#ifndef RT_H
#define RT_H

#define MAX_CALL_STACK 100

// 2MiB
#define STACK_SIZE 8*1024*1024

//16 MiB
#define BUFFER_SIZE 16 * 1024 * 1024

#define HEAP_SIZE 20

#define Capacity 0x1000

typedef unsigned long domain_id;

// A function call.
typedef struct Call {
	// Stack space.
	char stk[STACK_SIZE];

	// Next call.
	struct Call * next;
} Call;

typedef struct RSP {
	unsigned long rsp;
	domain_id caller;
	domain_id callee;
	unsigned long buffer_id;
} RSP;


void RT_init();
void MPK_init();
#endif
