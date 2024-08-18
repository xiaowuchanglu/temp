#include <stdint.h>

union IEEEl2bits {
	long double	e;
	struct {
		uint64_t	manl	:64;
		uint64_t	manh	:48;
		unsigned int	exp	:15;
		unsigned int	sign	:1;
	} bits;
	/* TODO andrew: Check the packing here */
	struct {
		uint64_t	manl	:64;
		uint64_t	manh	:48;
		unsigned int	expsign	:16;
	} xbits;
};

#define	LDBL_NBIT	0
#define	LDBL_IMPLICIT_NBIT
#define	mask_nbit_l(u)	((void)0)

#define	LDBL_MANH_SIZE	48
#define	LDBL_MANL_SIZE	64

#define	LDBL_TO_ARRAY32(u, a) do {			\
	(a)[0] = (uint32_t)(u).bits.manl;		\
	(a)[1] = (uint32_t)((u).bits.manl >> 32);	\
	(a)[2] = (uint32_t)(u).bits.manh;		\
	(a)[3] = (uint32_t)((u).bits.manh >> 32);	\
} while(0)
