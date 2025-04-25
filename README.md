# Userspace Trampoline Test

## Variants

- `ipc.off`: No trampoline (just `jmp`)
- `ipc.base`: Base trampoline
- `ipc.simd`: Base trampoline, plus FXSAVE and FXRSTOR in the caller side
- `ipc.regwipe`: Base trampoline, plus register wiping in both caller and callee
- `ipc.full`: Base trampoline, plus FXSAVE/FXRSTOR as well as register wiping (also performs VZEROALL)
