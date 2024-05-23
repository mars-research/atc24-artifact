default:
  just --list

# Figure 8: Overhead of Safe and Unsafe IPC (X86)
fig8-x86-64:
	./scripts/fig8-x86-64.sh

# Figure 8: Overhead of Safe and Unsafe IPC (AArch64)
fig8-aarch64:
	./scripts/fig8-aarch64.sh

# Figure 10: NF overheads on varying batch sizes (AArch64)
fig10:
	./scripts/fig10.sh

# Figure 11 - NF overheads on varying batch sizes (Morello)
fig11-morello:
	./scripts/fig11-morello.sh

# Figure 11 - NF overheads on varying batch sizes (Morello + Mainline)
fig11-mainline:
	./scripts/fig11-mainline.sh

# Figure 12: NF overheads on varying batch sizes (X86)
fig12:
	./scripts/fig12.sh

# LLVM

# Build Clang for X86-64
clang-x86-64:
	./scripts/build-clang-x86-64.sh

# Build Clang for AArch64
clang-aarch64:
	./scripts/build-clang-aarch64.sh

# SPEC

# Bootstrap spec-env
bootstrap-spec-env:
	./scripts/bootstrap-spec-env.sh

# Set up SPEC CPU2006
setup-spec2006:
	./scripts/setup-spec2006.sh

# Set up SPEC CPU2017
setup-spec2017:
	./scripts/setup-spec2017.sh

# Run SPEC CPU2006 on X86-64
spec2006-x86-64:
	./scripts/run-spec2006-x86-64.sh

# Run SPEC CPU2017 on X86-64
spec2017-x86-64:
	./scripts/run-spec2017-x86-64.sh

# Run SPEC CPU2006 on AArch64
spec2006-aarch64:
	./scripts/run-spec2006-aarch64.sh

# Run SPEC CPU2006 on Morello with Morello kernel
spec2006-morello:
	./scripts/run-spec2006-morello.sh

# Run SPEC CPU2017 on Morello with Morello kernel
spec2017-morello:
	./scripts/run-spec2017-morello.sh

# Run SPEC CPU2006 on Morello with Mainline kernel
spec2006-morello-mainline:
	./scripts/run-spec2006-morello-mainline.sh

# Run SPEC CPU2017 on Morello with Mainline kernel
spec2017-morello-mainline:
	./scripts/run-spec2017-morello-mainline.sh
