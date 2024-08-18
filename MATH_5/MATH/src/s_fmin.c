#include "cdefs-compat.h"
#include <MATH_math.h>

#include "fpmath.h"
#include "math_private.h"

OLM_DLLEXPORT double
fmin(double x, double y)
{
	union IEEEd2bits u[2];

	u[0].d = x;
	u[1].d = y;

	/* Check for NaNs to avoid raising spurious exceptions. */
	if (u[0].bits.exp == 2047 && (u[0].bits.manh | u[0].bits.manl) != 0)
		return (y);
	if (u[1].bits.exp == 2047 && (u[1].bits.manh | u[1].bits.manl) != 0)
		return (x);

	/* Handle comparisons of signed zeroes. */
	if (u[0].bits.sign != u[1].bits.sign)
		return (u[u[1].bits.sign].d);

	return (x < y ? x : y);
}
