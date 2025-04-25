#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include "rt.h"
#include "include.h"
#include "cet.h"
size_t batch_size = 32;

#define MTU 1500
#define CPU_FREQ 3800000000


static const char PACKET_TEMPLATE[] = {
	0x3c, 0xfd, 0xfe, 0xb4, 0xf9, 0xff, 0x3c, 0xfd, 0xfe, 0xb4, 0xfb, 0xdc, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x32, 0xa8, 0x2e, 0x00, 0x00, 0x40, 0x11, 0xb8, 0x76, 0x39, 0x55, 0x63, 0x02, 0x0a, 0x0a,
	0x03, 0x01, 0x15, 0xb3, 0x15, 0xb3, 0x00, 0x1e, 0x90, 0x00, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
};

uint64_t clock_gettime_latency = 0;
uint64_t fake_delay = 15;
uint64_t fake_per_delay = 360;
long int counter;

/// Receive packets from the synthetic generator.
size_t receive_packets(void *buf, size_t batch_size,void **buf_ptr) {
	for (size_t i = 0; i < batch_size; ++i) {
		char * pkt = (char*)buf + i * MTU;
		buf_ptr[i] = pkt;
		memcpy(pkt, PACKET_TEMPLATE, sizeof(PACKET_TEMPLATE));
		*(size_t*)(pkt+26) = counter%(1<<20);
		counter++;	
	}
	// for (size_t fd = 0; fd < (fake_delay + (fake_per_delay * batch_size)); ++fd) {
	// 	asm volatile("nop");
	// }
	return batch_size;
}

/// Reads the processor cycle counter.
uint64_t rdtsc() {
	uint32_t hi, lo;
	__asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)lo | (uint64_t)hi << 32);
}

/// Calibrates the latency of clock_gettime().
void calibrate_clock_gettime_latency() {
	// printf(stderr, "** Calibrating clock_gettime() latency... ");
	fflush(stderr);

	size_t iterations = 1000000;
	uint64_t start = rdtsc();

	struct timespec ts;
	for (size_t i = 0; i < iterations; ++i) {
		clock_gettime(CLOCK_MONOTONIC, &ts);
	}

	uint64_t cycles = rdtsc() - start;

	clock_gettime_latency = cycles / iterations;
	// printf(stderr, "%zu cycles\n", clock_gettime_latency);
}


char packets_a[MTU * 64]__attribute__ ( (section (".domain_a")) ) __attribute__ ( (aligned(4096)) );
char packets_b[MTU * 64]__attribute__ ( (section (".domain_b")) )__attribute__ ( (aligned(4096)) );
char packets_c[MTU * 64]__attribute__ ( (section (".domain_c")) )__attribute__ ( (aligned(4096)) );
char packets_d[MTU * 64]__attribute__ ( (section (".domain_d")) )__attribute__ ( (aligned(4096)) );

void *packets_a_ptr[64]__attribute__ ( (section (".domain_a")) )__attribute__ ( (aligned(4096)) );
void *packets_b_ptr[64]__attribute__ ( (section (".domain_b")) )__attribute__ ( (aligned(4096)) );
void *packets_c_ptr[64]__attribute__ ( (section (".domain_c")) )__attribute__ ( (aligned(4096)) );
void *packets_d_ptr[64]__attribute__ ( (section (".domain_d")) )__attribute__ ( (aligned(4096)) );

size_t (*_nf1)(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id)__attribute__ ( (section (".ro_trampoline_data")) );;
size_t (*_nf2)(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id)__attribute__ ( (section (".ro_trampoline_data")) );;
size_t (*_nf3)(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id)__attribute__ ( (section (".ro_trampoline_data")) );;
size_t (*_nf4)(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id)__attribute__ ( (section (".ro_trampoline_data")) );;
void (*nf4_init)();

void run_nfs_nacl(struct rte_ether_hdr *packets, uint64_t pkt_len) {

	memcpy(packets_a,packets,batch_size * MTU );
	
    nf1(packets_a_ptr,pkt_len,0,13);

	memcpy(packets_b,packets_a,batch_size * MTU );
    nf2(packets_b_ptr,pkt_len,0,13);

	memcpy(packets_c,packets_b,batch_size * MTU );
    nf3(packets_c_ptr,pkt_len,0,13);

	memcpy(packets_d,packets_c,batch_size * MTU );
    nf4(packets_d_ptr,pkt_len,0,13);

}

void run_nfs(struct rte_ether_hdr **packets, uint64_t pkt_len) {

	
    nf1(packets,pkt_len,0,13);
    nf2(packets,pkt_len,0,13);
    nf3(packets,pkt_len,0,13);
   	nf4(packets,pkt_len,0,13);

}



void Load_nf1(){
	void    *handle;
	handle = dlopen("./nf1.so", RTLD_LOCAL |RTLD_NOW);
	if(handle == NULL)
	{
    	// printf("Error: %s\n",  dlerror());
		abort();
	}
 	 _nf1 = dlsym(handle, "process_frames1");
 }
 void Load_nf2(){
	void    *handle;
	handle = dlopen("./nf2.so", RTLD_LOCAL |RTLD_NOW);
	if(handle == NULL)
	{
    	// printf("Error: %s\n",  dlerror());
		abort();
	}
 	 _nf2 = dlsym(handle, "process_frames2");
 }
 void Load_nf3(){
	void    *handle;
	handle = dlopen("./nf3.so", RTLD_LOCAL |RTLD_NOW);
	if(handle == NULL)
	{
    	// printf("Error: %s\n",  dlerror());
		abort();
	}
 	 _nf3 = dlsym(handle, "process_frames3");
 }
 void Load_nf4(){
	void    *handle;
	handle = dlopen("./nf4.so", RTLD_LOCAL |RTLD_NOW);
	if(handle == NULL)
	{
    	// printf("Error: %s\n",  dlerror());
		abort();
	}
 	 _nf4 = dlsym(handle, "process_frames4");
	 nf4_init = dlsym(handle, "nf4_init");
	asm("push %r15");
	asm("mov $0, %r15");
	 nf4_init();
	 asm("pop %r15");
 }

void init_nfs(){

	Load_nf1();
	//packets_a = malloc(MTU * batch_size);
	//packets_a_ptr = malloc(8 * batch_size);
	for (int i = 0; i < batch_size; i++){
		
		char * pkt = (char*)packets_a + i * MTU;
		packets_a_ptr[i] = pkt;
	}

    Load_nf2();
	//packets_b = malloc(MTU * batch_size);
	//packets_b_ptr = malloc(8 * batch_size);
	for (int i = 0; i < batch_size; i++){
		char * pkt = (char*)packets_b + i * MTU;
		packets_b_ptr[i] = pkt;
	}

    Load_nf3();
	//packets_c = malloc(MTU * batch_size);
	//packets_c_ptr = malloc(8 * batch_size);
	for (int i = 0; i < batch_size; i++){
		char * pkt = (char*)packets_c + i * MTU;
		packets_c_ptr[i] = pkt;
	}

	Load_nf4();
	//packets_d = malloc(MTU * batch_size);
	//packets_d_ptr = malloc(8 * batch_size);
	for (int i = 0; i < batch_size; i++){
		char * pkt = (char*)packets_d + i * MTU;
		packets_d_ptr[i] = pkt;
	}
}


/// Runs the receive loop.
void run_receive_loop() {
	size_t iterations = 10000000 / batch_size;

	void *packets = malloc(MTU * batch_size);
	void *packets_ptr[batch_size];

	struct timespec start_ts, end_ts;

	clock_gettime(CLOCK_MONOTONIC, &start_ts);
	// ENABLE_CET();
	uint64_t start = rdtsc();

	for (size_t i = 0; i < iterations; ++i) {
		receive_packets(packets, batch_size,packets_ptr);
		//run_nfs(packets_ptr,batch_size);
		run_nfs_nacl(packets, batch_size);
	}

	uint64_t cycles = rdtsc() - start;
	// DISABLE_CET();
	clock_gettime(CLOCK_MONOTONIC, &end_ts);
	uint64_t millis = (end_ts.tv_sec - start_ts.tv_sec) * 1000 + (end_ts.tv_nsec - start_ts.tv_nsec) / 1000000;
	uint64_t ppms = iterations * batch_size / millis;

	// printf("packets received = %zu\n", iterations * batch_size);
	// printf("    total cycles = %zu\n", cycles);
	// printf("      throughput = %zu pps\n", ppms * 1000);
	printf("%d, %zu\n",batch_size, ppms * 1000);
}


int main(int argc, char * argv[]) {
	if (argc != 2) {
		// printf(stderr, "Usage: %s [batch_size]", argv[0]);
		return 1;
	}
	batch_size = atoi(argv[1]);
    RT_init();
	MPK_init();	
	init_nfs();
	calibrate_clock_gettime_latency();
	run_receive_loop();
}