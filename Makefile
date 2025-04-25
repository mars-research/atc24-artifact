CROSS_COMPILE ?= 
CC := $(CROSS_COMPILE)clang
LLVM_DIR := /home/xiangdong/llvm_install_dir/arm_mte
SFICC := $(LLVM_DIR)/bin/clang
SFILD := $(LLVM_DIR)/bin/ld.lld
AS := $(CROSS_COMPILE)as
MARCH ?= armv9-a+memtag

ASFLAGS ?= -g
CFLAGS ?= -g -O3 -fPIC
ALL_CFLAGS ?= --target=aarch64-pc-linux -integrated-as -march=$(MARCH) $(CFLAGS)

.PHONY: all
all: fakegen.mte 

# fakegen: fakegen.o trampoline.o rt.o nf1.nosfi nf2.nosfi nf3.nosfi nf4.nosfi
# 	$(CC) -fPIC -o $@ $^

fakegen.mte: fakegen.o trampoline.o rt.o nf1.sfi nf2.sfi nf3.sfi nf4.sfi retag.o
	$(CC) -fPIC -o $@ $^

mte_test: mte_test.o retag.o  
	$(CC) -fPIC -o $@ $^

.PHONY: clean
clean:
	rm -f fakegen fakegen.mte *.o *.nosfi *.sfi 

%.o: %.S
	$(CC) -fPIC -E -CC -nostdinc $^ | $(AS) -march=$(MARCH) $(ASFLAGS) -o $@
#	$(CC) -c $(ALL_CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c -fPIC $(ALL_CFLAGS) -o $@ $^

%.nosfi: %.c
	$(CC) -c -fPIC $(ALL_CFLAGS) -o $@ $^

%.sfi: %.c
	$(SFICC) -O3 -fno-unwind-tables -fno-asynchronous-unwind-tables -fuse-ld=$(SFILD) $(ARM_CFLAGS) -c -o $@  $^	



