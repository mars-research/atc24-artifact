CC := cc
CXX := c++
NASM := nasm
CC_SFI := $(PWD)/../clang_install_dir/gs/bin
CFLAGS := -g -O3 -fPIC  -pedantic
CXXFLAGS := -g -O3 -fPIC -std=c++17 -pedantic

PROGRAM := rt.o fakegen.o
DOMAINS := nf1.so nf2.so nf3.so nf4.so

.PHONY: all
all: nfv_gs domains

domains: $(DOMAINS)

nfv_gs: trampoline.base.o $(PROGRAM)
	$(CC) -T ls.ld -o $@ $(CFLAGS) $^ -lpthread -ldl


%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.so: %.c
	$(CC_SFI)/clang -g -O3 -fno-stack-protector -shared -fPIC -fuse-ld=$(CC_SFI)/ld.lld -o $@ -nostdlib $< 

%.o: %.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

%.o: %.S trampoline.S
	$(NASM) -felf64 -o $@ $<


.PHONY: clean
clean:
	rm -f *.o ipc.* *.so
