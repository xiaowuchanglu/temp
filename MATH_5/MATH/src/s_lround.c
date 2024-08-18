#include "cdefs-compat.h"

#include <limits.h>
#include <MATH_fenv.h>
#include <MATH_math.h>

#include "math_private.h"

#ifndef type
#define type		double
#define	roundit		round
#define dtype		long
#define	DTYPE_MIN	LONG_MIN
#define	DTYPE_MAX	LONG_MAX
#define	fn		lround
#endif

/*
 * If type has more precision than dtype, the endpoints dtype_(min|max) are
 * of the form xxx.5; they are "out of range" because lround() rounds away
 * from 0.  On the other hand, if type has less precision than dtype, then
 * all values that are out of range are integral, so we might as well assume
 * that everything is in range.  At compile time, INRANGE(x) should reduce to
 * two floating-point comparisons in the former case, or TRUE otherwise.
 */
static const type dtype_min = DTYPE_MIN - 0.5;
static const type dtype_max = DTYPE_MAX + 0.5;
#define	INRANGE(x)	(dtype_max - DTYPE_MAX != 0.5 || \
			 ((x) > dtype_min && (x) < dtype_max))

OLM_DLLEXPORT dtype
fn(type x)
{

	if (INRANGE(x)) {
		x = roundit(x);
		return ((dtype)x);
	} else {
		feraiseexcept(FE_INVALID);
		return (DTYPE_MAX);
	}
}
