/*
 * Floating Point Data Structures and Constants
 * W. Jolitz 1/90
 */

#ifndef _BSD_FPU_H_
#define	_BSD_FPU_H_

#include "types-compat.h"

/* Environment information of floating point unit. */
struct env87 {
	int32_t		en_cw;		/* control word (16bits) */
	int32_t		en_sw;		/* status word (16bits) */
	int32_t		en_tw;		/* tag word (16bits) */
	int32_t		en_fip;		/* fp instruction pointer */
	uint16_t	en_fcs;		/* fp code segment selector */
	uint16_t	en_opcode;	/* opcode last executed (11 bits) */
	int32_t		en_foo;		/* fp operand offset */
	int32_t		en_fos;		/* fp operand segment selector */
};

/* Contents of each x87 floating point accumulator. */
struct fpacc87 {
	uint8_t		fp_bytes[10];
};

/* Floating point context. (i386 fnsave/frstor) */
struct save87 {
	struct env87	sv_env;		/* floating point control/status */
	struct fpacc87	sv_ac[8];	/* accumulator contents, 0-7 */
	uint8_t		sv_pad0[4];	/* saved status word (now unused) */
	/*
	 * Bogus padding for emulators.  Emulators should use their own
	 * struct and arrange to store into this struct (ending here)
	 * before it is inspected for ptracing or for core dumps.  Some
	 * emulators overwrite the whole struct.  We have no good way of
	 * knowing how much padding to leave.  Leave just enough for the
	 * GPL emulator's i387_union (176 bytes total).
	 */
	uint8_t		sv_pad[64];	/* padding; used by emulators */
};

/* Contents of each SSE extended accumulator. */
struct xmmacc {
	uint8_t		xmm_bytes[16];
};

/* Contents of the upper 16 bytes of each AVX extended accumulator. */
struct ymmacc {
	uint8_t		ymm_bytes[16];
};

/* Rename structs below depending on machine architecture. */
#ifdef __i386__
#define	__envxmm32	envxmm
#else
#define	__envxmm32	envxmm32
#define	__envxmm64	envxmm
#endif

struct __envxmm32 {
	uint16_t	en_cw;		/* control word (16bits) */
	uint16_t	en_sw;		/* status word (16bits) */
	uint16_t	en_tw;		/* tag word (16bits) */
	uint16_t	en_opcode;	/* opcode last executed (11 bits) */
	uint32_t	en_fip;		/* fp instruction pointer */
	uint16_t	en_fcs;		/* fp code segment selector */
	uint16_t	en_pad0;	/* padding */
	uint32_t	en_foo;		/* fp operand offset */
	uint16_t	en_fos;		/* fp operand segment selector */
	uint16_t	en_pad1;	/* padding */
	uint32_t	en_mxcsr;	/* SSE control/status register */
	uint32_t	en_mxcsr_mask;	/* valid bits in mxcsr */
};

struct __envxmm64 {
	uint16_t	en_cw;		/* control word (16bits) */
	uint16_t	en_sw;		/* status word (16bits) */
	uint8_t		en_tw;		/* tag word (8bits) */
	uint8_t		en_zero;
	uint16_t	en_opcode;	/* opcode last executed (11 bits ) */
	uint64_t	en_rip;		/* fp instruction pointer */
	uint64_t	en_rdp;		/* fp operand pointer */
	uint32_t	en_mxcsr;	/* SSE control/status register */
	uint32_t	en_mxcsr_mask;	/* valid bits in mxcsr */
};

/* Floating point context. (i386 fxsave/fxrstor) */
struct savexmm {
	struct __envxmm32	sv_env;
	struct {
		struct fpacc87	fp_acc;
		uint8_t		fp_pad[6];      /* padding */
	} sv_fp[8];
	struct xmmacc		sv_xmm[8];
	uint8_t			sv_pad[224];
} __attribute__ ((aligned(16)));

#ifdef __i386__
union savefpu {
	struct save87	sv_87;
	struct savexmm	sv_xmm;
};
#else
/* Floating point context. (amd64 fxsave/fxrstor) */
struct savefpu {
	struct __envxmm64	sv_env;
	struct {
		struct fpacc87	fp_acc;
		uint8_t		fp_pad[6];	/* padding */
	} sv_fp[8];
	struct xmmacc		sv_xmm[16];
	uint8_t			sv_pad[96];
} __attribute__ ((aligned(16)));
#endif

struct xstate_hdr {
	uint64_t	xstate_bv;
	uint8_t		xstate_rsrv0[16];
	uint8_t		xstate_rsrv[40];
};

struct savexmm_xstate {
	struct xstate_hdr	sx_hd;
	struct ymmacc		sx_ymm[16];
};

struct savexmm_ymm {
	struct __envxmm32	sv_env;
	struct {
		struct fpacc87	fp_acc;
		int8_t		fp_pad[6];	/* padding */
	} sv_fp[8];
	struct xmmacc		sv_xmm[16];
	uint8_t			sv_pad[96];
	struct savexmm_xstate	sv_xstate;
} __attribute__ ((aligned(16)));

struct savefpu_xstate {
	struct xstate_hdr	sx_hd;
	struct ymmacc		sx_ymm[16];
};

struct savefpu_ymm {
	struct __envxmm64	sv_env;
	struct {
		struct fpacc87	fp_acc;
		int8_t		fp_pad[6];	/* padding */
	} sv_fp[8];
	struct xmmacc		sv_xmm[16];
	uint8_t			sv_pad[96];
	struct savefpu_xstate	sv_xstate;
} __attribute__ ((aligned(64)));

#undef __envxmm32
#undef __envxmm64

/*
 * The hardware default control word for i387's and later coprocessors is
 * 0x37F, giving:
 *
 *	round to nearest
 *	64-bit precision
 *	all exceptions masked.
 *
 * FreeBSD/i386 uses 53 bit precision for things like fadd/fsub/fsqrt etc
 * because of the difference between memory and fpu register stack arguments.
 * If its using an intermediate fpu register, it has 80/64 bits to work
 * with.  If it uses memory, it has 64/53 bits to work with.  However,
 * gcc is aware of this and goes to a fair bit of trouble to make the
 * best use of it.
 *
 * This is mostly academic for AMD64, because the ABI prefers the use
 * SSE2 based math.  For FreeBSD/amd64, we go with the default settings.
 */
#define	__INITIAL_FPUCW__	0x037F
#define	__INITIAL_FPUCW_I386__	0x127F
#define	__INITIAL_NPXCW__	__INITIAL_FPUCW_I386__
#define	__INITIAL_MXCSR__	0x1F80
#define	__INITIAL_MXCSR_MASK__	0xFFBF

#endif /* !_BSD_FPU_H_ */
