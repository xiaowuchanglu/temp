#include <MATH_math.h>

#include "fpmath.h"
#include "math_private.h"

/* Provided by libc */
#if 1
OLM_DLLEXPORT int
(isnan) (double d)
{
	union IEEEd2bits u;

	u.d = d;
	return (u.bits.exp == 2047 && (u.bits.manl != 0 || u.bits.manh != 0));
}
#endif
