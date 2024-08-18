
#ifndef	_FENV_H_
#define	_FENV_H_

#include <stdint.h>

#include "cdefs-compat.h"

#ifndef	__fenv_static
#define	__fenv_static	static
#endif

typedef struct {
    uint32_t __fpscr; // Floating-Point Status and Control Register
} fenv_t;
typedef	uint32_t	fexcept_t;

/* Exception flags */
#define FE_INVALID    0x0001
#define FE_DIVBYZERO  0x0002
#define FE_OVERFLOW   0x0004
#define FE_UNDERFLOW  0x0008
#define FE_INEXACT    0x0010
#define	FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_INEXACT | \
			 FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define FE_TONEAREST  0x0000
#define FE_UPWARD     0x0001
#define FE_DOWNWARD   0x0002
#define FE_TOWARDZERO 0x0003
#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)
__BEGIN_DECLS

/* Default floating-point environment */
extern const fenv_t	__fe_dfl_env;
#define	FE_DFL_ENV	(&__fe_dfl_env)

/* We need to be able to map status flag positions to mask flag positions */
#define _FPUSW_SHIFT	16
#define	_ENABLE_MASK	(FE_ALL_EXCEPT << _FPUSW_SHIFT)

#if defined(__aarch64__)
#define __rfs(__fpsr)   __asm __volatile("mrs %0,fpsr" : "=r" (*(__fpsr)))
#define __wfs(__fpsr)   __asm __volatile("msr fpsr,%0" : : "r" (__fpsr))
/* Test for hardware support for ARM floating point operations, explicitly
checking for float and double support, see "ARM C Language Extensions", 6.5.1 */
#elif defined(__ARM_FP) && (__ARM_FP & 0x0C) != 0
#define __rfs(__fpsr)   __asm __volatile("vmrs %0,fpscr" : "=&r" (*(__fpsr)))
#define __wfs(__fpsr)   __asm __volatile("vmsr fpscr,%0" : : "r" (__fpsr))
#else
#define __rfs(__fpsr)	(*(__fpsr) = 0)
#define __wfs(__fpsr)
#endif

// Function prototypes
int feclearexcept(int __excepts);
int fetestexcept(int __excepts);
int fegetround(void);
int fesetround(int __round);

// Implementations
__fenv_static inline int
feclearexcept(int __excepts) {
    fenv_t env;
    // Assuming a function to read FPSCR into env.__fpscr
    read_fpscr(&env.__fpscr);
    env.__fpscr &= ~__excepts;
    // Assuming a function to write env.__fpscr back into FPSCR
    write_fpscr(env.__fpscr);
    return 0;
}

__fenv_static inline int
fegetexceptflag(fexcept_t *__flagp, int __excepts)
{
	fexcept_t __fpsr;

	__rfs(&__fpsr);
	*__flagp = __fpsr & __excepts;
	return (0);
}

__fenv_static inline int
fesetexceptflag(const fexcept_t *__flagp, int __excepts)
{
	fexcept_t __fpsr;

	__rfs(&__fpsr);
	__fpsr &= ~__excepts;
	__fpsr |= *__flagp & __excepts;
	__wfs(__fpsr);
	return (0);
}

__fenv_static inline int
feraiseexcept(int __excepts)
{
	fexcept_t __ex = __excepts;

	fesetexceptflag(&__ex, __excepts);	/* XXX */
	return (0);
}

__fenv_static inline int
fetestexcept(int __excepts) {
    fenv_t env;
    // Assuming a function to read FPSCR into env.__fpscr
    read_fpscr(&env.__fpscr);
    return env.__fpscr & __excepts;
}

__fenv_static inline int
fegetround(void) {
    fenv_t env;
    // Assuming a function to read FPSCR into env.__fpscr
    read_fpscr(&env.__fpscr);
    return env.__fpscr & _ROUND_MASK;
}

__fenv_static inline int
fesetround(int __round) {
    if (__round & ~_ROUND_MASK) return -1; // Invalid round value
    fenv_t env;
    // Assuming a function to read FPSCR into env.__fpscr
    read_fpscr(&env.__fpscr);
    env.__fpscr &= ~_ROUND_MASK;
    env.__fpscr |= __round;
    // Assuming a function to write env.__fpscr back into FPSCR
    write_fpscr(env.__fpscr);
    return 0;
}

__fenv_static inline int
fegetenv(fenv_t *__envp)
{

	__rfs(__envp);
	return (0);
}

__fenv_static inline int
feholdexcept(fenv_t *__envp)
{
	fenv_t __env;

	__rfs(&__env);
	*__envp = __env;
	__env &= ~(FE_ALL_EXCEPT | _ENABLE_MASK);
	__wfs(__env);
	return (0);
}

__fenv_static inline int
fesetenv(const fenv_t *__envp)
{

	__wfs(*__envp);
	return (0);
}

__fenv_static inline int
feupdateenv(const fenv_t *__envp)
{
	fexcept_t __fpsr;

	__rfs(&__fpsr);
	__wfs(*__envp);
	feraiseexcept(__fpsr & FE_ALL_EXCEPT);
	return (0);
}

#if __BSD_VISIBLE

/* We currently provide no external definitions of the functions below. */

static inline int
feenableexcept(int __mask)
{
	fenv_t __old_fpsr, __new_fpsr;

	__rfs(&__old_fpsr);
	__new_fpsr = __old_fpsr | (__mask & FE_ALL_EXCEPT) << _FPUSW_SHIFT;
	__wfs(__new_fpsr);
	return ((__old_fpsr >> _FPUSW_SHIFT) & FE_ALL_EXCEPT);
}

static inline int
fedisableexcept(int __mask)
{
	fenv_t __old_fpsr, __new_fpsr;

	__rfs(&__old_fpsr);
	__new_fpsr = __old_fpsr & ~((__mask & FE_ALL_EXCEPT) << _FPUSW_SHIFT);
	__wfs(__new_fpsr);
	return ((__old_fpsr >> _FPUSW_SHIFT) & FE_ALL_EXCEPT);
}

static inline int
fegetexcept(void)
{
	fenv_t __fpsr;

	__rfs(&__fpsr);
	return ((__fpsr & _ENABLE_MASK) >> _FPUSW_SHIFT);
}

#endif /* __BSD_VISIBLE */

__END_DECLS

#endif	/* !_FENV_H_ */