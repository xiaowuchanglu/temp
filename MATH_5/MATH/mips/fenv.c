#define	__fenv_static
#include "openlibm_fenv.h"

#ifdef __GNUC_GNU_INLINE__
#error "This file must be compiled with C99 'inline' semantics"
#endif

/*
 * Hopefully the system ID byte is immutable, so it's valid to use
 * this as a default environment.
 */
const fenv_t __fe_dfl_env = 0;

#ifdef __mips_soft_float
#define __set_env(env, flags, mask, rnd) env = ((flags)                 \
                                                | (mask)<<_FPUSW_SHIFT  \
                                                | (rnd) << 24)
#define __env_flags(env)                ((env) & FE_ALL_EXCEPT)
#define __env_mask(env)                 (((env) >> _FPUSW_SHIFT)        \
                                                & FE_ALL_EXCEPT)
#define __env_round(env)                (((env) >> 24) & _ROUND_MASK)
#include "fenv-softfloat.h"
#endif

extern inline int feclearexcept(int __excepts);
extern inline int fegetexceptflag(fexcept_t *__flagp, int __excepts);
extern inline int fesetexceptflag(const fexcept_t *__flagp, int __excepts);
extern inline int feraiseexcept(int __excepts);
extern inline int fetestexcept(int __excepts);
extern inline int fegetround(void);
extern inline int fesetround(int __round);
extern inline int fegetenv(fenv_t *__envp);
extern inline int feholdexcept(fenv_t *__envp);
extern inline int fesetenv(const fenv_t *__envp);
extern inline int feupdateenv(const fenv_t *__envp);
extern inline int feenableexcept(int __mask);
extern inline int fedisableexcept(int __mask);
extern inline int fegetexcept(void);

