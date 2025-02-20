

#include "bsd_fpu.h"
#include "math_private.h"

#ifdef _WIN32
#define __fenv_static OLM_DLLEXPORT
#endif
#include <MATH_fenv.h>

#ifdef __GNUC_GNU_INLINE__
#error "This file must be compiled with C99 'inline' semantics"
#endif

const fenv_t __fe_dfl_env = {
	{ 0xffff0000 | __INITIAL_FPUCW__,
	  0xffff0000,
	  0xffffffff,
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff }
	},
	__INITIAL_MXCSR__
};

extern inline OLM_DLLEXPORT int feclearexcept(int __excepts);
extern inline OLM_DLLEXPORT int fegetexceptflag(fexcept_t *__flagp, int __excepts);

OLM_DLLEXPORT int
fesetexceptflag(const fexcept_t *flagp, int excepts)
{
	fenv_t env;

	__fnstenv(&env.__x87);
	env.__x87.__status &= ~excepts;
	env.__x87.__status |= *flagp & excepts;
	__fldenv(env.__x87);

	__stmxcsr(&env.__mxcsr);
	env.__mxcsr &= ~excepts;
	env.__mxcsr |= *flagp & excepts;
	__ldmxcsr(env.__mxcsr);

	return (0);
}

OLM_DLLEXPORT int
feraiseexcept(int excepts)
{
	fexcept_t ex = excepts;

	fesetexceptflag(&ex, excepts);
	__fwait();
	return (0);
}

extern inline OLM_DLLEXPORT int fetestexcept(int __excepts);
extern inline OLM_DLLEXPORT int fegetround(void);
extern inline OLM_DLLEXPORT int fesetround(int __round);

OLM_DLLEXPORT int
fegetenv(fenv_t *envp)
{

	__fnstenv(&envp->__x87);
	__stmxcsr(&envp->__mxcsr);
	/*
	 * fnstenv masks all exceptions, so we need to restore the
	 * control word to avoid this side effect.
	 */
	__fldcw(envp->__x87.__control);
	return (0);
}

OLM_DLLEXPORT int
feholdexcept(fenv_t *envp)
{
	uint32_t mxcsr;

	__stmxcsr(&mxcsr);
	__fnstenv(&envp->__x87);
	__fnclex();
	envp->__mxcsr = mxcsr;
	mxcsr &= ~FE_ALL_EXCEPT;
	mxcsr |= FE_ALL_EXCEPT << _SSE_EMASK_SHIFT;
	__ldmxcsr(mxcsr);
	return (0);
}

extern inline OLM_DLLEXPORT int fesetenv(const fenv_t *__envp);

OLM_DLLEXPORT int
feupdateenv(const fenv_t *envp)
{
	uint32_t mxcsr;
	uint16_t status;

	__fnstsw(&status);
	__stmxcsr(&mxcsr);
	fesetenv(envp);
	feraiseexcept((mxcsr | status) & FE_ALL_EXCEPT);
	return (0);
}

int
feenableexcept(int mask)
{
	uint32_t mxcsr, omask;
	uint16_t control;

	mask &= FE_ALL_EXCEPT;
	__fnstcw(&control);
	__stmxcsr(&mxcsr);
	omask = ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
	control &= ~mask;
	__fldcw(control);
	mxcsr &= ~(mask << _SSE_EMASK_SHIFT);
	__ldmxcsr(mxcsr);
	return (omask);
}

int
fedisableexcept(int mask)
{
	uint32_t mxcsr, omask;
	uint16_t control;

	mask &= FE_ALL_EXCEPT;
	__fnstcw(&control);
	__stmxcsr(&mxcsr);
	omask = ~(control | mxcsr >> _SSE_EMASK_SHIFT) & FE_ALL_EXCEPT;
	control |= mask;
	__fldcw(control);
	mxcsr |= mask << _SSE_EMASK_SHIFT;
	__ldmxcsr(mxcsr);
	return (omask);
}
