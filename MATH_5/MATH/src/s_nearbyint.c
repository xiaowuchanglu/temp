#include "cdefs-compat.h"
#include <MATH_fenv.h>
#include <MATH_math.h>

#include "math_private.h"

/*
 * We save and restore the floating-point environment to avoid raising
 * an inexact exception.  We can get away with using fesetenv()
 * instead of feclearexcept()/feupdateenv() to restore the environment
 * because the only exception defined for rint() is overflow, and
 * rounding can't overflow as long as emax >= p.
 */
#define	DECL(type, fn, rint)	\
OLM_DLLEXPORT type				\
fn(type x)			\
{				\
	type ret;		\
	fenv_t env;		\
				\
	fegetenv(&env);		\
	ret = rint(x);		\
	fesetenv(&env);		\
	return (ret);		\
}

DECL(double, nearbyint, rint)
