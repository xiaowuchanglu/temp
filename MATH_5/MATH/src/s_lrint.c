

#include "cdefs-compat.h"

#include <MATH_fenv.h>
#include <MATH_math.h>

#include "math_private.h"

#ifndef type
#define type		double
#define	roundit		rint
#define dtype		long
#define	fn		lrint
#endif

/*
 * C99 says we should not raise a spurious inexact exception when an
 * invalid exception is raised.  Unfortunately, the set of inputs
 * that overflows depends on the rounding mode when 'dtype' has more
 * significant bits than 'type'.  Hence, we bend over backwards for the
 * sake of correctness; an MD implementation could be more efficient.
 */
OLM_DLLEXPORT dtype
fn(type x)
{
	fenv_t env;
	dtype d;

	feholdexcept(&env);
	d = (dtype)roundit(x);
	if (fetestexcept(FE_INVALID))
		feclearexcept(FE_INEXACT);
	feupdateenv(&env);
	return (d);
}
