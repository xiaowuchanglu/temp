#include <MATH_math.h>

#include "fpmath.h"
#include "math_private.h"

OLM_DLLEXPORT int
__isfinite(double d)
{
	union IEEEd2bits u;

	u.d = d;
	return (u.bits.exp != 2047);
}
