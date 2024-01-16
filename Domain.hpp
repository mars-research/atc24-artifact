#pragma once

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>

// Layout:
//
// Reserved   @ 0x0
// Stacks     @ 0x10000 (from 64 KiB)
// ELF Base   @ 0x10000000 (from 256 MiB)

#define DOMAIN_SIZE (4ULL * 1024 * 1024 * 1024) // 4 GiB

#define STACK_SIZE (4ULL * 1024 * 1024) // 4 MiB
#define STACK_OFFSET (16ULL * 4 * 1024) // 64 KiB (reserved)
#define STACK_NUM 10

#define ELF_BASE_OFFSET (256ULL * 1024 * 1024) // 256 MiB

typedef Elf64_Ehdr Ehdr;
typedef Elf64_Phdr Phdr;
typedef Elf64_Dyn Dyn;
typedef Elf64_Rel Rel;
typedef Elf64_Rela Rela;
#define R_TYPE(i) ELF64_R_TYPE(i)

#ifdef __aarch64__
#define R_RELATIVE R_AARCH64_RELATIVE
#elif defined(__x86_64__)
#define R_RELATIVE R_X86_64_RELATIVE
#else
#error Domain.hpp: Unsupported platform
#endif

struct Alignment {
	uint64_t alignment;

	Alignment(uint64_t alignment) : alignment(alignment) {}

	uint64_t inline alignUp(uint64_t addr) const {
		return (addr + alignment - 1) & ~(alignment - 1);
	}

	uint64_t inline alignDown(uint64_t addr) const {
		return addr & ~(alignment - 1);
	}

	uint64_t inline offset(uint64_t addr) const {
		return addr & (alignment - 1);
	}
};

template<class T, class CTX>
class Domain {
public:
	static constexpr const char *TYPE = T::TYPE;

	Domain(CTX *context, uint64_t id, std::string name, const char *path)
		: context(context), id(id), page_size(4096), name(name)
	{
		size = DOMAIN_SIZE;
		void *mapped = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

		if (mapped == MAP_FAILED) {
			throw std::runtime_error("Failed to reserve memory");
		}

		base = (uint64_t)mapped;

		std::cerr << "[domain] Loading " << name << " @ " << mapped << "\n";

		(static_cast<T*>(this))->preLoad_impl();

		void *stacks = mmap(
			(void*)((uint8_t*)mapped + STACK_OFFSET),
			STACK_SIZE * STACK_NUM,
			PROT_NONE,
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
			-1,
			0
		);
		if (stacks == MAP_FAILED) {
			throw std::runtime_error("Failed to map stacks");
		}
		fprintf(stderr, "[domain] Stacks @ %p\n", stacks);

		if (this->mprotect(stacks, STACK_SIZE * STACK_NUM, PROT_READ | PROT_WRITE) < 0) {
			fprintf(stderr, "Failed to protect stacks (%s)\n", strerror(errno));
			std::runtime_error("Failed to protect stacks");
		}

		for (auto i = 0; i < STACK_NUM; ++i) {
			uint64_t stack_top = this->getStack(i);
			uint64_t stack_bottom = stack_top + STACK_SIZE;
			uint64_t initial_sp = Alignment(16).alignDown(stack_bottom);

			volatile uint64_t *sp_save = (uint64_t*)stack_top;
			*sp_save = initial_sp;
		}

		fprintf(stderr, "[domain] First stack bottom @ 0x%lx\n", Alignment(16).alignDown(this->getStack(0) + STACK_SIZE));

		this->load(path);
		(static_cast<T*>(this))->postLoad_impl();
	}

	~Domain() {
		std::cerr << "[domain] Unloading " << name << "\n";
		munmap((void*)base, size);
	}

	// cannot be copied
	Domain(const Domain&) = delete;
	Domain& operator=(const Domain&) = delete;

	uint64_t start(uint64_t thread_id, uint64_t arg) {
		return (static_cast<T*>(this))->start_impl(thread_id, arg);
	}

	void setSlot(uint64_t slot_id, T &callee) {
		return (static_cast<T*>(this))->setSlot_impl(slot_id, callee);
	}

	void setSlotTcb(uint64_t slot_id, void *callee) {
		return (static_cast<T*>(this))->setSlotTcb_impl(slot_id, callee);
	}

	void invoke() {
		std::cerr << "trying to invoke\n";

		uint64_t (*f)(uint64_t, uint64_t) = (uint64_t (*)(uint64_t, uint64_t))entry_point;
		auto result = f(233, 233);
		std::cerr << "ret: " << result << "\n";
	}

protected:
	CTX *context;

	uint64_t id;

	uint64_t base;
	uint64_t size; // 4 GiB

	uint64_t load_bias;
	uint64_t entry_point;

	Alignment page_size;

	std::string name;

	void preLoad_impl() {
	}

	int mprotect(void *addr, size_t len, int prot) {
		return (static_cast<T*>(this))->mprotect_impl(addr, len, prot);
	}

	int mprotect_impl(void *addr, size_t len, int prot) {
		return ::mprotect(addr, len, prot);
	}

	void load(const char *path) {
		const int fd = open(path, O_RDONLY, 0);

		if (fd < 0) {
			std::cerr << "Failed to open " << path << ": " << std::strerror(errno) << "\n";
			abort();
		}

		Ehdr header;
		if (read(fd, &header, sizeof(header)) < 0) {
			std::cerr << "Failed to read ELF header of " << path << ": " << std::strerror(errno) << "\n";
			abort();
		}

		if (memcmp(header.e_ident, ELFMAG, SELFMAG)) {
			std::cerr << path << " is not a valid ELF file\n";
			abort();
		}

		if (header.e_type != ET_DYN) {
			std::cerr << path << " is not a dynamic library\n";
			abort();
		}

		if (header.e_phentsize != sizeof(Phdr)) {
			std::cerr << path << " has bad program header size\n";
			abort();
		}

		auto phs = ProgramHeaders(fd, header.e_phnum);
		auto summary = phs.summarizeLoadable();

		load_bias = this->getElfBase()/* - page_size.alignDown(summary.first_vaddr)*/;
		entry_point = load_bias + header.e_entry;

		fprintf(stderr, "[domain] Total mapping size: 0x%lx\n", summary.total_mapping_size);
		fprintf(stderr, "[domain]        First vaddr: 0x%lx\n", summary.first_vaddr);
		fprintf(stderr, "[domain]          Load bias: 0x%lx\n", load_bias);
		fprintf(stderr, "[domain]        Entry point: 0x%lx\n", entry_point);

		for (size_t i = 0; i < phs.phnum; ++i) {
			Phdr ph = phs.ptr()[i];

			if (ph.p_type == PT_DYNAMIC) {
				Dynamic dyn(fd, ph, load_bias, page_size);
				dyn.fixup();
			}

			if (ph.p_type != PT_LOAD || ph.p_memsz == 0) {
				continue;
			}

			uint64_t memsz = ph.p_memsz;
			uint64_t filesz = ph.p_filesz;
			uint64_t vaddr = ph.p_vaddr;
			uint64_t vend = vaddr + memsz;
			uint64_t fend = vaddr + filesz;
			uint64_t offset = ph.p_offset;

			fprintf(stderr, "[domain] Mapping segment @ 0x%lx\n", vaddr);

			int prot = (ph.p_flags & PF_R ? PROT_READ : 0)
				| (ph.p_flags & PF_W ? PROT_WRITE : 0)
				| (ph.p_flags & PF_X ? PROT_EXEC : 0);

			uint64_t total_map_size = page_size.alignUp(vend) - page_size.alignDown(vaddr);
			uint64_t file_map_size = page_size.alignUp(std::min(fend, vend)) - page_size.alignDown(vaddr);

			if (file_map_size) {
				// Assumption: pageOffset(ph.p_vaddr) == pageOffset(ph.p_offset)
				void *mapping = mmap(
					(void*)page_size.alignDown(load_bias + vaddr),
					file_map_size,
					PROT_NONE,
					MAP_PRIVATE | MAP_FIXED,
					fd,
					page_size.alignDown(offset)
				);

				if (mapping == MAP_FAILED) {
					fprintf(stderr, "Failed to map segment 0x%lx (%s)\n", vaddr, strerror(errno));
					throw std::runtime_error("Failed to map segment");
				}

				if (this->mprotect(mapping, file_map_size, prot) < 0) {
					fprintf(stderr, "Failed to protect segment 0x%lx (%s)\n", vaddr, strerror(errno));
					throw std::runtime_error("Failed to protect segment");
				}
			}

			// Memory beyond filesz is zero-initialized
			if (memsz > filesz && (ph.p_flags & PF_W) != 0) {
				uint64_t zero_addr = load_bias + vaddr + filesz;
				uint64_t zero_end = page_size.alignUp(zero_addr);

				if (zero_end > zero_addr) {
					memset((void*)zero_addr, 0, zero_end - zero_addr);
				}

				if (file_map_size < total_map_size) {
					void *mapping = mmap(
						(void*)(page_size.alignDown(load_bias + vaddr) + file_map_size),
						total_map_size - file_map_size,
						PROT_NONE,
						MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
						-1,
						0
					);

					if (mapping == MAP_FAILED) {
						fprintf(stderr, "Failed to map anonymous portion for segment 0x%lx (%s)\n", vaddr, strerror(errno));
						throw std::runtime_error("Failed to map anonymous portion");
					}

					if (this->mprotect(mapping, total_map_size - file_map_size, prot) < 0) {
						fprintf(stderr, "Failed to protect segment 0x%lx (%s)\n", vaddr, strerror(errno));
						throw std::runtime_error("Failed to protect anonymous portion");
					}
				}
			}
		}
	}

	uint64_t getElfBase() const {
		return base + ELF_BASE_OFFSET;
	}

	uint64_t getStack(uint64_t thread_id) const {
		return base + STACK_OFFSET + thread_id * STACK_SIZE;
	}

	struct LoadableSummary {
		uint64_t first_vaddr;
		uint64_t total_mapping_size;
	};

	struct ProgramHeaders {
		void *mapped;
		size_t map_size;
		size_t phnum;

		ProgramHeaders(const int fd, size_t phnum) : phnum(phnum) {
			map_size = sizeof(Ehdr) + sizeof(Phdr) * phnum;
			mapped = mmap(nullptr, map_size, PROT_READ, MAP_PRIVATE, fd, 0);

			if (mapped == MAP_FAILED) {
				throw std::runtime_error("Failed to map program headers");
			}
		}
		~ProgramHeaders() {
			munmap(mapped, map_size);
		}

		Phdr *ptr() const {
			return (Phdr*)((uint8_t*)mapped + sizeof(Ehdr));
		}

		LoadableSummary summarizeLoadable() {
			LoadableSummary summary;
			uint64_t addr_min = UINT64_MAX;
			uint64_t addr_max = 0;

			Phdr *phs = this->ptr();
			for (size_t i = 0; i < phnum; ++i) {
				Phdr ph = phs[i];
				if (ph.p_type != PT_LOAD || ph.p_memsz == 0) {
					continue;
				}

				if (addr_min > ph.p_vaddr) {
					addr_min = ph.p_vaddr;
				}

				uint64_t vend = ph.p_vaddr + ph.p_memsz;
				if (addr_max < vend) {
					addr_max = vend;
				}
			}

			summary.first_vaddr = addr_min;
			summary.total_mapping_size = addr_max - addr_min;

			return summary;
		}
	};

	struct Dynamic {
		Dyn *base;
		uint64_t load_bias;
		Alignment page_size;

		Dynamic(const int fd, Phdr &ph, uint64_t load_bias, Alignment &page_size)
			: load_bias(load_bias), page_size(page_size)
		{
			if (ph.p_type != PT_DYNAMIC) {
				throw std::runtime_error("Not a PT_DYNAMIC segment");
			}

			base = (Dyn*)((uint8_t*)load_bias + ph.p_vaddr);
		}

		void fixup() {
			Rela *rela = nullptr;
			size_t rela_len = 0;

			Rel *rel = nullptr;
			size_t rel_len = 0;

			for (Dyn *cur = base; cur->d_tag != DT_NULL; ++cur) {
				switch (cur->d_tag) {
					case DT_RELA:
						rela = (Rela*)((uint8_t*)load_bias + cur->d_un.d_ptr);
						break;
					case DT_RELASZ:
						rela_len = (size_t)cur->d_un.d_val / sizeof(Rela);
						break;
					case DT_RELAENT:
						if (cur->d_un.d_val != sizeof(Rela)) {
							throw std::runtime_error("DT_RELAENT has unsupported size");
						}
						break;

					case DT_REL:
						rel = (Rel*)((uint8_t*)load_bias + cur->d_un.d_ptr);
						break;
					case DT_RELSZ:
						rel_len = (size_t)cur->d_un.d_val / sizeof(Rel);
						break;
					case DT_RELENT:
						if (cur->d_un.d_val != sizeof(Rel)) {
							throw std::runtime_error("DT_RELENT has unsupported size");
						}
						break;

					case DT_PLTGOT:
						throw std::runtime_error("GOT/PLT is not implemented");
						break;
					default:
						break;
				}
			}

			if (rela) {
				for (size_t i = 0; i < rela_len; ++i) {
					Rela *cur = rela + i;
					auto r_type = R_TYPE(cur->r_info);

					if (r_type != R_RELATIVE) {
						throw std::runtime_error("Unsupported relocation");
					}

					volatile uint64_t *ptr = (volatile uint64_t*)((uint8_t*)load_bias + cur->r_offset);
					*ptr = load_bias + cur->r_addend;
				}
			}

			if (rel) {
				for (size_t i = 0; i < rel_len; ++i) {
					Rel *cur = rel + i;
					auto r_type = R_TYPE(cur->r_info);

					if (r_type != R_RELATIVE) {
						throw std::runtime_error("Unsupported relocation");
					}

					volatile uint64_t *ptr = (volatile uint64_t*)((uint8_t*)load_bias + cur->r_offset);
					uint64_t addend = *ptr;
					*ptr = load_bias + addend;
				}
			}
		}
	};
};
