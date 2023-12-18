#ifdef __CHERI_PURE_CAPABILITY__
#error Must be compiled for Hybrid mode (PSTATE.C64=0)
#endif

.text

// Simple capability return trampoline
// Set x9 to destination

str c30, [sp, -16]!
blr x9
ldr c30, [sp], 16
rets
