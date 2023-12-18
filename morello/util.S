#ifdef __CHERI_PURE_CAPABILITY__
#error Must be compiled for Hybrid mode (PSTATE.C64=0)
#endif

# void *__capability get_ddc_cur()
.global get_ddc_cur
get_ddc_cur:
	mrs c0, ddc
	ret

# void store_cap(void *dest, void *__capability cap)
.global store_cap
store_cap:
	str c1, [x0]
	ret
