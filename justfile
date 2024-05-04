default:
  just --list

# Run all experiments on X86-64
run-x86-64: fig8

# Figure 8: Overhead of Safe and Unsafe IPC
fig8:
	./scripts/fig8-invocation-chain.sh
