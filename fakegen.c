#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rt.h"
#include "packettool.h"
#include <dlfcn.h>
#define MTU 1500
#define CPU_FREQ 3800000000
void *packets;
void *packets_ptr;
size_t batch_size = 32;

void mte_retag_dcgva(void *base_with_tag, size_t size);

size_t process_frames1(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id);
size_t process_frames2(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id);
size_t process_frames3(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id);
size_t process_frames4(struct rte_ether_hdr** packets,int nb_rx, int not_used,int buffer_id);
void init_nf2();
void init_nf3();

extern char _domain_A_start;
extern char _domain_A_end;
extern char _domain_B_start;
extern char _domain_B_end;
extern char _domain_C_start;
extern char _domain_C_end;
extern char _domain_D_start;
extern char _domain_D_end;

void run_nfs(struct rte_ether_hdr **packets, uint64_t pkt_len) {

	mte_retag_dcgva(packets,round_2_64(MTU*batch_size));
	mte_retag_dcgva(packets_ptr,round_2_64(8*batch_size));
    nf1(packets,pkt_len,0,0);

	mte_retag_dcgva(packets,round_2_64(MTU*batch_size));
	mte_retag_dcgva(packets_ptr,round_2_64(8*batch_size));
    nf2(packets,pkt_len,0,0);

	mte_retag_dcgva(packets,round_2_64(MTU*batch_size));
	mte_retag_dcgva(packets_ptr,round_2_64(8*batch_size));
    nf3(packets,pkt_len,0,0);

	mte_retag_dcgva(packets,round_2_64(MTU*batch_size));
	mte_retag_dcgva(packets_ptr,round_2_64(8*batch_size));
    nf4(packets,pkt_len,0,0);

}

void init_nfs(){
	init_nf2();
	init_nf3();
	nf4_init();
}


static const char PACKET_TEMPLATE[] = {
	0x3c, 0xfd, 0xfe, 0xb4, 0xf9, 0xff, 0x3c, 0xfd, 0xfe, 0xb4, 0xfb, 0xdc, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x32, 0xa8, 0x2e, 0x00, 0x00, 0x40, 0x11, 0xb8, 0x76, 0x39, 0x55, 0x63, 0x02, 0x0a, 0x0a,
	0x03, 0x01, 0x15, 0xb3, 0x15, 0xb3, 0x00, 0x1e, 0x90, 0x00, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
};

uint64_t clock_gettime_latency = 0;
uint64_t fake_delay = 0;
uint64_t fake_per_delay = 0;
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
	return batch_size;
}




uint64_t prev_tsc, diff_tsc, cur_tsc, timer_tsc;
/// Runs the receive loop.
void run_receive_loop() {
	size_t iterations = 10000000 / batch_size;

	struct timespec start_ts, end_ts;

	clock_gettime(CLOCK_MONOTONIC, &start_ts);

	for (size_t i = 0; i < iterations; ++i) {
		receive_packets(packets, batch_size,packets_ptr);
		run_nfs(packets_ptr,batch_size);
	}

	clock_gettime(CLOCK_MONOTONIC, &end_ts);
	uint64_t millis = (end_ts.tv_sec - start_ts.tv_sec) * 1000 + (end_ts.tv_nsec - start_ts.tv_nsec) / 1000000;
	uint64_t ppms = iterations * batch_size / millis;

	printf("packets received = %zu\n", iterations * batch_size);
	printf("      throughput = %zu pps\n", ppms * 1000);
}


int main(int argc, char * argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [batch_size]", argv[0]);
		return 1;
	}
	batch_size = atoi(argv[1]);
	packets = aligned_alloc(4096,round_2_64(MTU*batch_size));
	packets_ptr = aligned_alloc(4096,round_2_64(8*batch_size));
	Enable_MTE();
	void * end = (size_t)packets + MTU*batch_size;
	void * st = packets;
	while(st < end){
		Enable_MTE_on_page(st);
		st = (size_t)st + 4096;
	}
	// printf("_domain_A_start: %lx\n_domain_A_end: %lx\n",&_domain_A_start,&_domain_A_end);
	// st = &_domain_A_start;
	// while(st < &_domain_A_end){
	// 	Enable_MTE_on_page(st);
	// 	st = (size_t)st + 4096;
	// }
	// printf("_domain_B_start: %lx\n_domain_B_end: %lx\n",&_domain_B_start,&_domain_B_end);
	// st = &_domain_B_start;
	// while(st < &_domain_B_end){
	// 	Enable_MTE_on_page(st);
	// 	st = (size_t)st + 4096;
	// }
	// printf("_domain_C_start: %lx\n_domain_C_end: %lx\n",&_domain_C_start,&_domain_C_end);
	// st = &_domain_C_start;
	// while(st < &_domain_C_end){
	// 	Enable_MTE_on_page(st);
	// 	st = (size_t)st + 4096;
	// }
	// printf("_domain_D_start: %lx\n_domain_D_end: %lx\n",&_domain_D_start,&_domain_D_end);
	// st = &_domain_D_start;
	// while(st < &_domain_D_end){
	// 	Enable_MTE_on_page(st);
	// 	st = (size_t)st + 4096;
	// }
	Enable_MTE_on_page(packets_ptr);
	mte_retag(packets,round_2_64(MTU*batch_size));
	mte_retag(packets_ptr,round_2_64(8*batch_size));
    RT_init();
	MTE_init();
	init_nfs();
	run_receive_loop();
}